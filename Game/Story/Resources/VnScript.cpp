// Game/Story/Resources/VnScript.cpp
#include "VnScript.h"
#include "Utils/FileUtils.h"

#include <sstream>

namespace Salt2D::Game::Story {

static inline bool IsCommentOrEnpty(const std::string& line) {
    for (char ch : line) {
        if (ch == '#') return true;
        if (!std::isspace(static_cast<unsigned char>(ch))) return false;
    }
    return true;
}

static inline bool LooksLikeCue(const std::string& line) {
    // = something =
    return line.size() >= 3 && line.front() == '=' && line.back() == '=';
}

static bool ParseQuotedLine(const std::string& line, std::string& outSpeaker, std::string& outText) {
    // speaker "text"
    // speaker 「text」
    // speaker "text"
    
    // Define quote pairs (UTF-8 encoded)
    struct QuotePair {
        std::string opening;
        std::string closing;
    };
    
    const QuotePair quotes[] = {
        {"\"", "\""},           // ASCII double quote
        {"\xe3\x80\x8c", "\xe3\x80\x8d"},  // 「」 Japanese quotes
        {"\xe2\x80\x9c", "\xe2\x80\x9d"}   // "" curly quotes
    };
    
    // Find the first opening quote
    size_t quotePos = std::string::npos;
    const QuotePair* selectedQuote = nullptr;
    
    for (const auto& quote : quotes) {
        size_t pos = line.find(quote.opening);
        if (pos != std::string::npos && (quotePos == std::string::npos || pos < quotePos)) {
            quotePos = pos;
            selectedQuote = &quote;
        }
    }
    
    if (quotePos == std::string::npos || !selectedQuote) {
        return false;  // No opening quote found
    }
    
    // Extract speaker (everything before the quote)
    std::string speaker = line.substr(0, quotePos);
    // Trim whitespace
    Utils::Trim(speaker);
    if (speaker.empty()) {
        return false;  // No speaker
    }
    
    // Find closing quote
    size_t textStart = quotePos + selectedQuote->opening.size();
    size_t closingPos = line.find(selectedQuote->closing, textStart);
    
    if (closingPos == std::string::npos) {
        return false;  // No closing quote
    }
    
    // Extract text between quotes
    std::string text = line.substr(textStart, closingPos - textStart);
    
    outSpeaker = std::move(speaker);
    outText = std::move(text);
    return true;
}

VnScript ParseVnScriptText(const std::string& utf8Text) {
    VnScript script;
    
    std::istringstream iss(utf8Text);
    std::string line;
    while (std::getline(iss, line)) {
        Utils::Trim(line);
        if (line.empty()) continue;
        if (IsCommentOrEnpty(line)) continue;

        if (LooksLikeCue(line)) {
            std::string inner = line.substr(1, line.size() - 2);;
            Utils::Trim(inner);
            VnCmd cmd;
            cmd.type = VnCmdType::Cue;
            cmd.cueName = std::move(inner);
            script.cmds.push_back(std::move(cmd));
            continue;
        }

        std::string speaker, text;
        if (ParseQuotedLine(line, speaker, text)) {
            VnCmd cmd;
            cmd.type = VnCmdType::Line;
            cmd.speaker = std::move(speaker);
            cmd.text = std::move(text);
            script.cmds.push_back(std::move(cmd));
            continue;
        }

        // If we reach here, the line format is unrecognized
        throw std::runtime_error("VnScriptParser: unrecognized line format: " + line);
    }
    
    return script;
}

} // namespace Salt2D::Game::Story
