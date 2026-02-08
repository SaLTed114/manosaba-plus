// Game/Sceens/DebateScreen.cpp
#include "DebateScreen.h"

#include "Game/Session/StoryActions.h"
#include "Game/Session/StoryHistory.h"
#include "Game/Story/StoryPlayer.h"
#include "Game/RenderBridge/TextService.h"
#include "Game/RenderBridge/TextureService.h"
#include "Render/Draw/DrawList.h"
#include "Utils/MathUtils.h"

#include <Windows.h>

namespace Salt2D::Game::Screens {

void DebateScreen::HandleKeyboard(Session::ActionFrame& af) {
    const auto& view = player_->View().debate;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    const auto navY = af.actions.ConsumeNavY();
    const auto confirm = af.actions.ConsumeConfirm();
    const auto cancel = af.actions.ConsumeCancel();
    const auto openMenu = af.actions.debateOpenMenuPressed;

    if (view->menuOpen && cancel) {
        player_->CloseDebateMenu();
        if (history_) history_->Push(Story::NodeType::Debate,
            Session::HistoryKind::MenuBack, "", "");
        return;
    }

    if (navY < 0) {
        if (view->menuOpen) --selectedOption_;
        else --selectedSpan_;
    }
    if (navY > 0) {
        if (view->menuOpen) ++selectedOption_;
        else ++selectedSpan_;
    }

    if (!confirm && !openMenu) return;

    if (!view->menuOpen) {
        if (!view->spanIds.empty() && openMenu) {
            const int index = Utils::ClampWarp(selectedSpan_, static_cast<int>(view->spanIds.size()));
            const std::string spanId = view->spanIds[index];
            player_->OpenSuspicion(spanId);

            const auto& updatedView = player_->View().debate;
            if (updatedView.has_value() && updatedView->menuOpen) {
                std::string line = "Options: ";
                for (const auto& [id, label] : updatedView->options) line += "| " + label;
                line += " | [Back]";
                if (history_) history_->Push(Story::NodeType::Debate,
                    Session::HistoryKind::MenuOpen, "", line, updatedView->openedSpanId);
            }
        }

        if (confirm) {
            player_->Advance();
            LogHistory();
        }
    } else {
        if (!view->options.empty() && confirm) {
            const int index = Utils::ClampWarp(selectedOption_, static_cast<int>(view->options.size()));
            const auto& option = view->options[index];
            
            const std::string optionId = option.first;
            const std::string optionLabel = option.second;
            
            player_->CommitOption(optionId);     
            if (history_) history_->Push(Story::NodeType::Debate,
                Session::HistoryKind::OptionPick, "", optionLabel, optionId);
            LogHistory();
        }
    }
}

void DebateScreen::HandlePointer(Session::ActionFrame& af) {
    if (!dialog_.Visible()) return;
    const auto interaction = UI::UIInteraction::Update(frame_, af, pointer_);
    dialog_.ApplyHover(frame_, interaction.hovered);
}

void DebateScreen::BuildUI(uint32_t canvasW, uint32_t canvasH) {
    frame_.Clear();
    dialog_.SetVisible(false);

    const auto& view = player_->View().debate;
    if (!view.has_value()) { dialog_.SetVisible(false); return; }

    Game::UI::DebateHudModel model;
    model.visible = true;
    model.speakerUtf8  = view->speaker;
    model.bodyUtf8     = view->fullText;
    model.spanIds      = view->spanIds;
    model.menuOpen     = view->menuOpen;
    model.openedSpanId = view->openedSpanId;
    model.menuOptions  = view->options;

    model.selectedSpan = Utils::ClampWarp(selectedSpan_,   static_cast<int>(view->spanIds.size()));
    model.selectedOpt  = Utils::ClampWarp(selectedOption_, static_cast<int>(view->options.size()));

    dialog_.Build(model, canvasW, canvasH, frame_);
}

void DebateScreen::Tick(Session::ActionFrame& af, uint32_t canvasW, uint32_t canvasH) {
    if (!player_) { frame_.Clear(); dialog_.SetVisible(false); return; }

    if (kbEnabled_) HandleKeyboard(af);
    BuildUI(canvasW, canvasH);
}

void DebateScreen::Bake(const RHI::DX11::DX11Device& device, RenderBridge::TextService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;
    if (!theme_) return;

    baker_.Bake(device, service, frame_);
    dialog_.AfterBake(frame_);
}

void DebateScreen::PostBake(Session::ActionFrame& af, uint32_t /*canvasW*/, uint32_t /*canvasH*/) {
    if (!player_) return;
    if (!dialog_.Visible()) return;

    HandlePointer(af);
}

void DebateScreen::EmitDraw(Render::DrawList& drawList, RenderBridge::TextureService& service) {
    if (!player_) return;
    if (!dialog_.Visible()) return;

    emitter_.Emit(drawList, service, frame_);
}

void DebateScreen::LogHistory() {
    if (!player_ || !history_) return;
    const auto& view = player_->View().debate;
    if (!view.has_value()) return;

    // Log each statement once when it first appears.
    if (view->statementIndex <= lastStmtInx_) return;
    lastStmtInx_ = view->statementIndex;

    history_->Push(Story::NodeType::Debate, view->speaker, view->fullText);
}

void DebateScreen::OnEnter() {
    selectedSpan_ = 0;
    selectedOption_ = 0;
    lastStmtInx_ = -283;

    pointer_ = {};
    dialog_.SetVisible(false);
    baker_.SetTheme(theme_);
    LogHistory();
}

void DebateScreen::OnExit() {
    dialog_.SetVisible(false);
}

} // namespace Salt2D::Game::Screens
