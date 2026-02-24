// Render/DX11DebugLogger.h
#ifndef RENDER_DX11DEBUGLOGGER_H
#define RENDER_DX11DEBUGLOGGER_H

#include <wrl/client.h>
#include <d3d11.h>
#include <d3d11sdklayers.h>
#include <string>
#include <memory>
#include <fstream>
#include <mutex>

namespace Salt2D::Render {

class DX11DebugLogger {
public:
    DX11DebugLogger() = default;
    ~DX11DebugLogger();

    // Initialize with ID3D11Device, returns true if debug layer is available
    bool Initialize(ID3D11Device* device);

    // Check and output all pending D3D11 debug messages
    void FlushMessages();

    // Enable/disable certain message categories
    void SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY severity, bool enable);
    void FilterOutInfo(bool filterOut);

    bool IsEnabled() const { return infoQueue_ != nullptr; }

private:
    void LogMessage(const char* severity, const std::string& category, const std::string& message);

private:
    Microsoft::WRL::ComPtr<ID3D11InfoQueue> infoQueue_;
    UINT64 lastMessageIndex_ = 0;
    
    std::unique_ptr<std::ofstream> logFile_;
    std::unique_ptr<std::mutex> logMutex_;
};

} // namespace Salt2D::Render

#endif // RENDER_DX11DEBUGLOGGER_H
