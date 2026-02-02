// Game/Story/Runners/VnRunner.cpp
#include "VnRunner.h"

#include <stdexcept>

namespace Salt2D::Game::Story {

VnRunner::VnRunner(Utils::IFileSystem& fs) : fs_(fs) {}

void VnRunner::Enter(const Node& node) {
    if (node.resourceFullPath.empty()) {
        throw std::runtime_error("VnRunner: node resource path is empty");
    }
    const std::string text = fs_.ReadTextFileUtf8(node.resourceFullPath, true);
    script_ = ParseVnScriptText(text);

    cmdIndex_ = 0;
    state_ = VnState{};
    LoadNextLineOrFinish();
}

std::optional<GraphEvent> VnRunner::Advance() {
    if (state_.finished) return GraphEvent{Trigger::Auto, ""};

    if (!state_.lineDone) {
        const size_t totalCp = CountCodepoints(state_.fullText);
        if (state_.revealed < totalCp) {
            state_.revealed++;
            if (logger_) {
                std::string currentText = Utf8PrefixByCodepoints(state_.fullText, state_.revealed);
                logger_->Debug("VnRunner", 
                    "Advance: " + state_.speaker + ": " + currentText + 
                    " [" + std::to_string(state_.revealed) + "/" + std::to_string(totalCp) + "]");
            }
        }
        state_.lineDone = (state_.revealed >= totalCp);
        return std::nullopt;
    }

    LoadNextLineOrFinish();
    if (state_.finished) {
        if (logger_) {
            logger_->Debug("VnRunner", "Advance: Finished script");
        }
        return GraphEvent{Trigger::Auto, ""};
    }
    return std::nullopt;
}

std::optional<GraphEvent> VnRunner::SkipLine() {
    if (state_.finished) return GraphEvent{Trigger::Auto, ""};

    const size_t totalCp = CountCodepoints(state_.fullText);
    state_.revealed = totalCp;
    state_.lineDone = true;
    if (logger_) {
        logger_->Debug("VnRunner",
            "SkipLine: " + state_.speaker + ": " + state_.fullText);
    }
    return std::nullopt;
}

std::optional<GraphEvent> VnRunner::FastForwardAll() {
    if (logger_) {
        logger_->Debug("VnRunner", "FastForwardAll: Starting fast forward...");
    }
    while (!state_.finished) {
        const size_t totalCp = CountCodepoints(state_.fullText);
        state_.revealed = totalCp;
        state_.lineDone = true;
        if (logger_) {
            logger_->Debug("VnRunner",
                "FastForwardAll: " + state_.speaker + ": " + state_.fullText);
        }
        LoadNextLineOrFinish();
    }
    if (logger_) {
        logger_->Debug("VnRunner",
            "FastForwardAll: Finished script");
    }
    return GraphEvent{Trigger::Auto, ""};
}

void VnRunner::LoadNextLineOrFinish() {
    while (cmdIndex_ < script_.cmds.size()) {
        const VnCmd& cmd = script_.cmds[cmdIndex_++];
        if (cmd.type == VnCmdType::Cue) {
            if (onCue_) onCue_(cmd.cueName);
            else if (logger_) logger_->Debug("VnRunner", "Cue: " + cmd.cueName);
            continue;
        }

        state_.speaker  = cmd.speaker;
        state_.fullText = cmd.text;
        state_.revealed = 0;
        state_.lineDone = false;
        state_.finished = false;
        if (logger_) {
            logger_->Debug("VnRunner",
                "LoadNextLine: New line loaded - " +
                state_.speaker + ": \"" + state_.fullText + "\"");
        }
        return;
    }
    
    state_.finished = true;
    if (logger_) {
        logger_->Debug("VnRunner",
            "LoadNextLineOrFinish: No more lines, finished");
    }
}

size_t VnRunner::CountCodepoints(const std::string& utf8) const {
    size_t cnt = 0;
    for (unsigned char c : utf8) {
        if ((c & 0xC0) != 0x80) cnt++;
    }
    return cnt;
}

std::string VnRunner::Utf8PrefixByCodepoints(const std::string& utf8, size_t cpCount) const {
    if (cpCount == 0) return "";
    
    size_t cnt = 0;
    size_t byteIndex = 0;
    for (unsigned char c : utf8) {
        if ((c & 0xC0) != 0x80) {
            if (cnt >= cpCount) break;
            cnt++;
        }
        byteIndex++;
    }
    return utf8.substr(0, byteIndex);
}

} // namespace Salt2D::Game::Story
