// Render/DX11DebugLogger.cpp
#include "DX11DebugLogger.h"
#include "Utils/FileUtils.h"
#include <d3d11sdklayers.h>
#include <vector>
#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace Salt2D::Render {

static const char* SeverityToString(D3D11_MESSAGE_SEVERITY severity) {
    switch (severity) {
    case D3D11_MESSAGE_SEVERITY_CORRUPTION: return "ERROR";
    case D3D11_MESSAGE_SEVERITY_ERROR:      return "ERROR";
    case D3D11_MESSAGE_SEVERITY_WARNING:    return "WARN";
    case D3D11_MESSAGE_SEVERITY_INFO:       return "INFO";
    case D3D11_MESSAGE_SEVERITY_MESSAGE:    return "INFO";
    default:                                return "INFO";
    }
}

static std::string CategoryToString(D3D11_MESSAGE_CATEGORY category) {
    switch (category) {
    case D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED: return "Application";
    case D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:       return "Miscellaneous";
    case D3D11_MESSAGE_CATEGORY_INITIALIZATION:      return "Initialization";
    case D3D11_MESSAGE_CATEGORY_CLEANUP:             return "Cleanup";
    case D3D11_MESSAGE_CATEGORY_COMPILATION:         return "Compilation";
    case D3D11_MESSAGE_CATEGORY_STATE_CREATION:      return "StateCreation";
    case D3D11_MESSAGE_CATEGORY_STATE_SETTING:       return "StateSetting";
    case D3D11_MESSAGE_CATEGORY_STATE_GETTING:       return "StateGetting";
    case D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION: return "Resource";
    case D3D11_MESSAGE_CATEGORY_EXECUTION:           return "Execution";
    case D3D11_MESSAGE_CATEGORY_SHADER:              return "Shader";
    default:                                        return "Unknown";
    }
}

bool DX11DebugLogger::Initialize(ID3D11Device* device) {
    if (!device) return false;

    HRESULT hr = device->QueryInterface(IID_PPV_ARGS(&infoQueue_));
    if (FAILED(hr)) {
        return false;
    }

    // Configure to not break on warnings by default
    infoQueue_->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, TRUE);
    infoQueue_->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, FALSE);
    infoQueue_->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, FALSE);

    // Get starting message count
    lastMessageIndex_ = infoQueue_->GetNumStoredMessages();

    // Initialize log file
    logMutex_ = std::make_unique<std::mutex>();
    
    // Generate timestamped log filename
    std::string logPath = Utils::GenerateTimestampedFilename("Logs/d3d11_debug.log");
    
    std::filesystem::path logPathFs(logPath);
    if (logPathFs.has_parent_path()) {
        std::error_code ec;
        std::filesystem::create_directories(logPathFs.parent_path(), ec);
    }
    
    logFile_ = std::make_unique<std::ofstream>(logPath, std::ios::out | std::ios::binary);
    if (logFile_->is_open()) {
        LogMessage("INFO", "D3D11", "Debug Layer initialized");
    }
    
    return true;
}

DX11DebugLogger::~DX11DebugLogger() {
    if (logFile_ && logFile_->is_open()) {
        logFile_->close();
    }
}

void DX11DebugLogger::LogMessage(const char* severity, const std::string& category, const std::string& message) {
    using namespace std::chrono;
    
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm tm{};
#if defined(WIN32) || defined(_WIN32)
    localtime_s(&tm, &time);
#else
    localtime_r(&time, &tm);
#endif
    
    std::ostringstream oss;
    oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << '.' << std::setfill('0') << std::setw(3) << ms.count() << "]"
        << "[" << severity << "]"
        << "[" << category << "] "
        << message;
    
    std::string logLine = oss.str();
    
    // Output to console
    std::cout << logLine << std::endl;
    
    // Output to file
    if (logFile_ && logFile_->is_open() && logMutex_) {
        std::lock_guard<std::mutex> lock(*logMutex_);
        (*logFile_) << logLine << std::endl;
        logFile_->flush();
    }
}

void DX11DebugLogger::FlushMessages() {
    if (!infoQueue_) return;

    UINT64 messageCount = infoQueue_->GetNumStoredMessages();
    
    if (messageCount <= lastMessageIndex_) return;

    for (UINT64 i = lastMessageIndex_; i < messageCount; ++i) {
        SIZE_T messageLength = 0;
        HRESULT hr = infoQueue_->GetMessage(i, nullptr, &messageLength);
        if (FAILED(hr)) continue;

        std::vector<uint8_t> messageBuffer(messageLength);
        auto* message = reinterpret_cast<D3D11_MESSAGE*>(messageBuffer.data());

        hr = infoQueue_->GetMessage(i, message, &messageLength);
        if (FAILED(hr)) continue;

        const char* severityStr = SeverityToString(message->Severity);
        std::string category = CategoryToString(message->Category);
        std::string msg(message->pDescription, message->DescriptionByteLength - 1);
        
        LogMessage(severityStr, category, msg);
    }

    lastMessageIndex_ = messageCount;
}

void DX11DebugLogger::SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY severity, bool enable) {
    if (infoQueue_) {
        infoQueue_->SetBreakOnSeverity(severity, enable ? TRUE : FALSE);
    }
}

void DX11DebugLogger::FilterOutInfo(bool filterOut) {
    if (!infoQueue_) return;

    if (filterOut) {
        D3D11_MESSAGE_SEVERITY denySeverities[] = {
            D3D11_MESSAGE_SEVERITY_INFO,
            D3D11_MESSAGE_SEVERITY_MESSAGE
        };

        D3D11_INFO_QUEUE_FILTER filter{};
        filter.DenyList.NumSeverities = static_cast<UINT>(std::size(denySeverities));
        filter.DenyList.pSeverityList = denySeverities;

        infoQueue_->AddStorageFilterEntries(&filter);
    } else {
        infoQueue_->ClearStorageFilter();
    }
}

} // namespace Salt2D::Render
