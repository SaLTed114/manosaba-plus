// Resources/Mesh/MeshLoader.cpp
#include "MeshLoader.h"
#include <fstream>
#include <sstream>
#include <unordered_map>

namespace Salt2D::Resources {

namespace {

static int ResolveObjIndex(int index, int size) {
    if (index > 0) return index - 1;
    if (index < 0) return size + index;
    return -1; // 0 is invalid in OBJ indexing
}

struct ObjIndexTriplet {
    int v = -1;  // vertex index
    int vt = -1; // texture coordinate index
    int vn = -1; // normal index

    bool operator==(const ObjIndexTriplet& other) const {
        return v == other.v && vt == other.vt && vn == other.vn;
    }
};

struct TripletHash {
    std::size_t operator()(const ObjIndexTriplet& triplet) const noexcept {
        std::size_t h = 1469598103934665603ull;
        auto mix = [&](int x) {
            h ^= static_cast<std::size_t>(x + 1) + 0x9e3779b97f4a7c15ull + (h << 6) + (h >> 2);
        };
        mix(triplet.v);
        mix(triplet.vt);
        mix(triplet.vn);
        return h;
    }
};

static ObjIndexTriplet ParseObjIndex(const std::string& token) {
    ObjIndexTriplet triplet;
    
    int parts[3] = { 0, 0, 0 };
    bool has[3] = { false, false, false };

    int partIndex = 0;
    std::string currentPart;

    auto flushPart = [&]() {
        if (!currentPart.empty()) {
            parts[partIndex] = std::stoi(currentPart);
            has[partIndex] = true;
            currentPart.clear();
        }
    };

    for (auto ch : token) {
        if (ch == '/') {
            flushPart();
            partIndex++;
            if (partIndex > 2) break;
        } else {
            currentPart += ch;
        }
    }
    flushPart();

    if (!has[0]) throw std::runtime_error("Invalid OBJ index: " + token);

    triplet.v = parts[0];
    triplet.vt = has[1] ? parts[1] : 0;
    triplet.vn = has[2] ? parts[2] : 0;
    return triplet;
}

static DirectX::XMFLOAT3 ApplyOptionsToPosition(
    const DirectX::XMFLOAT3& pos,
    const MeshLoadOptions& options
) {
    DirectX::XMFLOAT3 result = pos;

    // Apply scaling
    result.x *= options.scale;
    result.y *= options.scale;
    result.z *= options.scale;

    // Flip Z if needed
    if (options.flipZ) result.z = -result.z;

    return result;
}

static void CenterAndNormalizeMesh(
    std::vector<Vertex>& vertices
) {
    if (vertices.empty()) return;

    DirectX::XMFLOAT3 minPos = vertices[0].position;
    DirectX::XMFLOAT3 maxPos = vertices[0].position;

    for (auto& vertex : vertices) {
        const auto& pos = vertex.position;
        minPos.x = std::min(minPos.x, pos.x);
        minPos.y = std::min(minPos.y, pos.y);
        minPos.z = std::min(minPos.z, pos.z);

        maxPos.x = std::max(maxPos.x, pos.x);
        maxPos.y = std::max(maxPos.y, pos.y);
        maxPos.z = std::max(maxPos.z, pos.z);
    }

    DirectX::XMFLOAT3 center = {
        (minPos.x + maxPos.x) * 0.5f,
        (minPos.y + maxPos.y) * 0.5f,
        (minPos.z + maxPos.z) * 0.5f
    };

    float maxExtent = std::max({ maxPos.x - minPos.x, maxPos.y - minPos.y, maxPos.z - minPos.z });
    float scale = (maxExtent <= 1e-6f) ? 1.0f : (1.0f / maxExtent);

    for (auto& vertex : vertices) {
        vertex.position.x = (vertex.position.x - center.x) * scale;
        vertex.position.y = (vertex.position.y - center.y) * scale;
        vertex.position.z = (vertex.position.z - center.z) * scale;
    }
}

static void GenerateNormals(
    std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices
) {
    // Initialize all normals to zero
    for (auto& vertex : vertices) {
        vertex.normal = { 0.0f, 0.0f, 0.0f };
    }

    // Accumulate face normals for each vertex
    for (size_t i = 0; i < indices.size(); i += 3) {
        uint32_t i0 = indices[i];
        uint32_t i1 = indices[i + 1];
        uint32_t i2 = indices[i + 2];

        const auto& p0 = vertices[i0].position;
        const auto& p1 = vertices[i1].position;
        const auto& p2 = vertices[i2].position;

        // Calculate face normal using cross product
        DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&p0);
        DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&p1);
        DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&p2);

        DirectX::XMVECTOR edge1 = DirectX::XMVectorSubtract(v1, v0);
        DirectX::XMVECTOR edge2 = DirectX::XMVectorSubtract(v2, v0);
        DirectX::XMVECTOR faceNormal = DirectX::XMVector3Cross(edge1, edge2);

        // Accumulate to each vertex (weighted by face area)
        DirectX::XMFLOAT3 fn;
        DirectX::XMStoreFloat3(&fn, faceNormal);

        vertices[i0].normal.x += fn.x;
        vertices[i0].normal.y += fn.y;
        vertices[i0].normal.z += fn.z;

        vertices[i1].normal.x += fn.x;
        vertices[i1].normal.y += fn.y;
        vertices[i1].normal.z += fn.z;

        vertices[i2].normal.x += fn.x;
        vertices[i2].normal.y += fn.y;
        vertices[i2].normal.z += fn.z;
    }

    // Normalize all vertex normals
    for (auto& vertex : vertices) {
        DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&vertex.normal);
        n = DirectX::XMVector3Normalize(n);
        DirectX::XMStoreFloat3(&vertex.normal, n);
    }
}

} // namespace

std::shared_ptr<MeshData> MeshLoader::LoadOBJ(
    const std::filesystem::path& path,
    const MeshLoadOptions& options
) {
    namespace fs = std::filesystem;
    fs::path resolvedPath = Utils::ResolvePath(path);

    std::ifstream file(resolvedPath);
    if (!file) {
        throw std::runtime_error("Failed to open OBJ file: " + resolvedPath.string());
    }

    std::vector<DirectX::XMFLOAT3> positions;
    std::vector<DirectX::XMFLOAT3> normals;
    std::vector<DirectX::XMFLOAT2> texCoords;

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    std::unordered_map<ObjIndexTriplet, uint32_t, TripletHash> vertexMap;
    vertexMap.reserve(1 << 16);

    bool needsNormalGeneration = false;

    std::string line;
    while (std::getline(file, line)) {
        Utils::Trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::istringstream iss(line);
        std::string tag;
        iss >> tag;

        if (tag == "v") {
            float x, y, z;
            iss >> x >> y >> z;
            positions.push_back({ x, y, z });
        } else if (tag == "vn") {
            float x, y, z;
            iss >> x >> y >> z;
            normals.push_back({ x, y, z });
        } else if (tag == "vt") {
            float u, v;
            iss >> u >> v;
            texCoords.push_back({ u, v });
        } else if (tag == "f") {
            std::vector<ObjIndexTriplet> face;
            std::string token;
            while (iss >> token) {
                face.push_back(ParseObjIndex(token));
            }
            if (face.size() < 3) continue;

            auto emitVertex = [&](ObjIndexTriplet tRaw) -> uint32_t {
                ObjIndexTriplet t = tRaw;

                t.v = ResolveObjIndex(tRaw.v, static_cast<int>(positions.size()));
                t.vt = (tRaw.vt == 0) ? -1 : ResolveObjIndex(tRaw.vt, static_cast<int>(texCoords.size()));
                t.vn = (tRaw.vn == 0) ? -1 : ResolveObjIndex(tRaw.vn, static_cast<int>(normals.size()));

                if (t.v < 0 || t.v >= static_cast<int>(positions.size())) {
                    throw std::runtime_error("Invalid vertex index in OBJ file.");
                }

                auto it = vertexMap.find(t);
                if (it != vertexMap.end()) return it->second;

                Vertex vertex;
                vertex.position = ApplyOptionsToPosition(positions[t.v], options);
                vertex.color = options.defaultColor;

                // Handle texture coordinates if available in OBJ file
                if (t.vt >= 0 && t.vt < static_cast<int>(texCoords.size())) {
                    auto uv = texCoords[t.vt];
                    if (options.flipV) uv.y = 1.0f - uv.y;
                    vertex.texCoord = uv;
                } else {
                    vertex.texCoord = { 0.0f, 0.0f };
                    needsNormalGeneration = true;
                }

                // Handle normals if available in OBJ file
                if (t.vn >= 0 && t.vn < static_cast<int>(normals.size())) {
                    vertex.normal = normals[t.vn];
                    if (options.flipZ) {
                        vertex.normal.z = -vertex.normal.z;
                    }
                } else {
                    vertex.normal = { 0.0f, 0.0f, 0.0f }; // Will be generated later
                }
                
                uint32_t newIndex = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
                vertexMap.emplace(t, newIndex);
                return newIndex;
            };

            auto tri = [&](uint32_t i0, uint32_t i1, uint32_t i2) {
                if (options.flipWinding) {
                    indices.push_back(i0);
                    indices.push_back(i2);
                    indices.push_back(i1);
                } else {
                    indices.push_back(i0);
                    indices.push_back(i1);
                    indices.push_back(i2);
                }
            };

            uint32_t v0 = emitVertex(face[0]);
            for (size_t i = 1; i + 1 < face.size(); i++) {
                uint32_t v1 = emitVertex(face[i]);
                uint32_t v2 = emitVertex(face[i + 1]);
                tri(v0, v1, v2);
            }
        } else {
            // Ignore other tags
            continue;
        }
    }

    if (options.centerAndNormalize) {
        CenterAndNormalizeMesh(vertices);
    }

    // Generate normals if OBJ file didn't have them
    if (normals.empty() || needsNormalGeneration) {
        GenerateNormals(vertices, indices);
    }

    auto meshData = std::make_shared<MeshData>();
    meshData->vertices = std::move(vertices);
    meshData->indices  = std::move(indices);
    return meshData;
}

} // namespace Salt2D::Resources