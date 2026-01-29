// Render/Passes/IRenderPass.h
#ifndef RENDER_PASSES_IRENDERPASS_H
#define RENDER_PASSES_IRENDERPASS_H

#include <string_view>

namespace Salt2D::Render {

struct PassContext;

class IRenderPass {
public:
    virtual ~IRenderPass() = default;
    virtual std::string_view Name() const = 0;
    virtual void Record(PassContext& ctx) = 0;
};

} // namespace Salt2D::Render

#endif // RENDER_PASSES_IRENDERPASS_H
