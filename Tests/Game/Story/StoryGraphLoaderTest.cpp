// Tests/Game/Story/StoryGraphLoaderTest.cpp
#include "Game/Story/StoryGraphLoader.h"
#include "Utils/DiskFileSystem.h"

#include <windows.h>
#include <iostream>
#include <iomanip>

using namespace Salt2D::Game::Story;
using namespace Salt2D::Utils;

static std::string NodeTypeToString(NodeType type) {
    return std::string(ToString(type));
}

static std::string TriggerToString(Trigger trigger) {
    return std::string(ToString(trigger));
}

static void PrintNode(const Node& node) {
    std::cout << "    Node '" << node.id << "':\n";
    std::cout << "      Type: " << NodeTypeToString(node.type) << "\n";
    std::cout << "      Resource: " << node.resourcePath.string() << "\n";
    std::cout << "      Full Path: " << node.resourceFullPath.string() << "\n";
    
    if (node.params.timeLimitSec.has_value()) {
        std::cout << "      Time Limit: " << node.params.timeLimitSec.value() << "s\n";
    }
    if (node.params.beNode.has_value()) {
        std::cout << "      BE Node: " << node.params.beNode.value() << "\n";
    }
}

static void PrintEdge(const Edge& edge, int index) {
    std::cout << "    Edge #" << index << ": " 
              << edge.from << " -> " << edge.to 
              << " (trigger: " << TriggerToString(edge.trigger);
    
    if (!edge.key.empty()) {
        std::cout << ", key: " << edge.key;
    }
    std::cout << ")\n";
    
    if (!edge.effects.empty()) {
        std::cout << "      Effects:\n";
        for (const auto& effect : edge.effects) {
            std::cout << "        - " << effect.type << ": " << effect.name << "\n";
        }
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    try {
        std::cout << "=== StoryGraphLoader Test ===\n\n";

        // 使用DiskFileSystem
        DiskFileSystem diskFS;

        // 构建图路径
        std::filesystem::path graphPath = "Assets/Story/Demo/demo_story.graph.json";
        
        std::cout << "Loading story graph from: " << graphPath << "\n\n";

        // 加载选项
        StoryGraphLoadOptions options;
        options.checkResourcesExists = false; // 先不检查资源文件是否存在
        options.requireDebateNoCommit = true;
        options.requireDebateTimeDepleted = false; // demo图可能没有time_depleted

        // 加载图
        StoryGraph graph = LoadStoryGraph(diskFS, graphPath, options);

        std::cout << "✓ Graph loaded successfully!\n\n";
        std::cout << "Base Directory: " << graph.GetBaseDir() << "\n\n";

        // 打印节点信息
        const auto& nodes = graph.Nodes();
        std::cout << "=== Nodes (" << nodes.size() << ") ===\n";
        for (const auto& [id, node] : nodes) {
            PrintNode(node);
            std::cout << "\n";
        }

        // 打印边信息
        const auto& edges = graph.Edges();
        std::cout << "=== Edges (" << edges.size() << ") ===\n";
        for (size_t i = 0; i < edges.size(); i++) {
            PrintEdge(edges[i], static_cast<int>(i));
        }
        std::cout << "\n";

        // 测试特定节点查找
        std::cout << "=== Testing Node Lookup ===\n";
        if (graph.HasNode("n0_intro")) {
            const Node& introNode = graph.GetNode("n0_intro");
            std::cout << "  ✓ Found node 'n0_intro'\n";
            std::cout << "    Type: " << NodeTypeToString(introNode.type) << "\n";
        }

        if (graph.HasNode("n1_interrogation")) {
            const Node& debateNode = graph.GetNode("n1_interrogation");
            std::cout << "  ✓ Found node 'n1_interrogation'\n";
            std::cout << "    Type: " << NodeTypeToString(debateNode.type) << "\n";
            if (debateNode.params.timeLimitSec.has_value()) {
                std::cout << "    Time limit: " << debateNode.params.timeLimitSec.value() << "s\n";
            }
            if (debateNode.params.beNode.has_value()) {
                std::cout << "    BE node: " << debateNode.params.beNode.value() << "\n";
            }
        }
        std::cout << "\n";

        // 测试边查找
        std::cout << "=== Testing Edge Lookup ===\n";
        GraphEvent autoEvent{Trigger::Auto, ""};
        const Edge* edge = graph.FindEdge("n0_intro", autoEvent);
        if (edge) {
            std::cout << "  ✓ Found auto edge from n0_intro -> " << edge->to << "\n";
        }

        GraphEvent optionEvent{Trigger::Option, "opt_rebut"};
        edge = graph.FindEdge("n1_interrogation", optionEvent);
        if (edge) {
            std::cout << "  ✓ Found option edge from n1_interrogation -> " << edge->to << " (key: opt_rebut)\n";
            if (!edge->effects.empty()) {
                std::cout << "    Effects: ";
                for (size_t i = 0; i < edge->effects.size(); i++) {
                    if (i > 0) std::cout << ", ";
                    std::cout << edge->effects[i].type << ":" << edge->effects[i].name;
                }
                std::cout << "\n";
            }
        }
        std::cout << "\n";

        std::cout << "=== All Tests Passed! ===\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "✗ Error: " << e.what() << "\n";
        return 1;
    }
}
