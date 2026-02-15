// Game/Story/Runners/VnRunner.cpp
#include "VnRunner.h"

#include <cmath>
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
        // auto mode: while not done, advance means reveal all immediately
        state_.revealed = lineTotalCp_;
        state_.lineDone = true;
        state_.revealCpF = static_cast<float>(lineTotalCp_);

        if (logger_) {
            logger_->Debug("VnRunner",
                "Advance: " + state_.speaker + ": " + state_.fullText);
        }
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

    state_.revealed = lineTotalCp_;
    state_.lineDone = true;
    if (logger_) {
        logger_->Debug("VnRunner",
            "SkipLine: " + state_.speaker + ": " + state_.fullText);
    }
    
    LoadNextLineOrFinish();
    if (state_.finished) {
        if (logger_) {
            logger_->Debug("VnRunner", "SkipLine: Finished script");
        }
        return GraphEvent{Trigger::Auto, ""};
    }
    return std::nullopt;
}

std::optional<GraphEvent> VnRunner::FastForwardAll() {
    if (logger_) {
        logger_->Debug("VnRunner", "FastForwardAll: Starting fast forward...");
    }
    while (!state_.finished) {
        state_.revealed = lineTotalCp_;
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

void VnRunner::Tick(float dtSec, float cps) {
    if (state_.finished || state_.lineDone) return;
    if (dtSec < 0.0f || cps <= 0.0f) return;

    revealAcc_ += dtSec * cps;
    state_.revealCpF += dtSec * cps;
    state_.revealCpF = (std::min)(state_.revealCpF, static_cast<float>(lineTotalCp_));

    const size_t add = static_cast<size_t>(std::floor(revealAcc_));
    if (add == 0) return;

    revealAcc_ -= static_cast<float>(add);
    
    state_.revealed = (std::min)(state_.revealed + add, lineTotalCp_);
    state_.lineDone = (state_.revealed >= lineTotalCp_);

    if (logger_) {
        std::string currentText = Utf8PrefixByCodepoints(state_.fullText, state_.revealed);
        logger_->Debug("VnRunner",
            "Tick: " + state_.speaker + ": " + currentText + 
            " [" + std::to_string(state_.revealed) + "/" + std::to_string(lineTotalCp_) + "]");
    }
}

void VnRunner::LoadNextLineOrFinish() {
    while (cmdIndex_ < script_.cmds.size()) {
        const VnCmd& cmd = script_.cmds[cmdIndex_++];
        if (cmd.type == VnCmdType::Cue) {
            if (onCue_) onCue_(cmd.cueName);
            else if (logger_) logger_->Debug("VnRunner", "Cue: " + cmd.cueName);
            continue;
        }

        lineTotalCp_ = CountCodepoints(cmd.text);
        revealAcc_ = 0.0f;

        state_.speaker  = cmd.speaker;
        state_.fullText = cmd.text;
        state_.revealed = 0;
        state_.totalCp  = lineTotalCp_;
        state_.lineDone = false;
        state_.finished = false;
        state_.lineSerial++;
        state_.revealCpF = 0.0f;

        if (lineTotalCp_ == 0) {
            state_.revealed = 0;
            state_.lineDone = true;
        }

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
