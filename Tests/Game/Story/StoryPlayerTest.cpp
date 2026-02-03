// Tests/Game/Story/StoryPlayerTest.cpp
#include "Game/Story/StoryGraphLoader.h"
#include "Game/Story/StoryPlayer.h"
#include "Utils/DiskFileSystem.h"
#include <iostream>
#include <windows.h>

using namespace Salt2D::Game::Story;

static std::string Utf8PrefixByCodepoints(const std::string& utf8, size_t cpCount) {
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

void DisplayView(const StoryView& view) {
    std::cout << "\n========== Story View ==========\n";
    std::cout << "Node Type: " << ToString(view.nodeType) << "\n";
    
    if (view.vn.has_value()) {
        const auto& vn = *view.vn;
        std::cout << "--- VN View ---\n";
        std::cout << "Speaker: " << vn.speaker << "\n";
        std::cout << "Full Text: " << vn.fullText << "\n";
        auto currentText = Utf8PrefixByCodepoints(vn.fullText, vn.revealed);
        auto totalCodepoints = std::count_if(vn.fullText.begin(), vn.fullText.end(), [](unsigned char c) {
            return (c & 0xC0) != 0x80;
        });
        std::cout << "Revealed: " << currentText << "[" + std::to_string(vn.revealed) + "/" + std::to_string(totalCodepoints) + "]" << "\n";
        std::cout << "Line Done: " << (vn.lineDone ? "Yes" : "No") << "\n";
        std::cout << "Finished: " << (vn.finished ? "Yes" : "No") << "\n";
    }
    
    if (view.present.has_value()) {
        const auto& present = *view.present;
        std::cout << "--- Present View ---\n";
        std::cout << "Prompt: " << present.prompt << "\n";
        std::cout << "Available Items:\n";
        for (size_t i = 0; i < present.items.size(); ++i) {
            std::cout << "  [" << i << "] " << present.items[i].first << " - " << present.items[i].second << "\n";
        }
    }
    
    if (view.debate.has_value()) {
        const auto& debate = *view.debate;
        std::cout << "--- Debate View ---\n";
        std::cout << "Statement: " << debate.statementIndex << "/" << debate.statementCount-1 << "\n";
        std::cout << "Speaker: " << debate.speaker << "\n";
        std::cout << "Text: " << debate.fullText << "\n";
        
        if (!debate.spanIds.empty()) {
            std::cout << "Available Spans:\n";
            for (size_t i = 0; i < debate.spanIds.size(); ++i) {
                std::cout << "  <" << i << "> " << debate.spanIds[i];
                if (debate.menuOpen && debate.spanIds[i] == debate.openedSpanId) {
                    std::cout << " [OPENED]";
                }
                std::cout << "\n";
            }
        }
        
        if (debate.menuOpen && !debate.options.empty()) {
            std::cout << "Menu Options (span: " << debate.openedSpanId << "):\n";
            for (size_t i = 0; i < debate.options.size(); ++i) {
                std::cout << "  [" << i << "] " << debate.options[i].first << " - " << debate.options[i].second << "\n";
            }
        }
    }
    std::cout << "================================\n";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    try {
        Salt2D::Utils::DiskFileSystem diskFS;
        auto graphPath = std::filesystem::path("Assets/Story/Demo/demo_story.graph.json");
        auto graph = LoadStoryGraph(diskFS, graphPath);
        std::cout << "Loaded story graph with " << graph.Nodes().size() << " nodes and " << graph.Edges().size() << " edges.\n\n";

        auto logger = Salt2D::Game::MakeConsoleAndFileLogger("Logs/story_player_test.log", Salt2D::Game::LogLevel::Info, Salt2D::Game::LogLevel::Debug);

        StoryPlayer player(graph, diskFS);
        player.SetEffectCallback([](const Effect& effect) {
            std::cout << "[Effect Callback] Effect applied: type='" << effect.type << "', name='" << effect.name << "'\n";
        });

        player.SetLogger(&logger);
        player.Start("n0_intro");

        /* ======
         * n        : Advance / Advance statement (Debate)
         * f        : FastForward
         * 1        : Commit option "opt_agree"
         * 2        : Commit option "opt_rebut"
         * p<id>    : Pick evidence by itemId (e.g., "pevid_pen")
         * #<N>     : Pick evidence by index from Present view
         * s<N>     : Open suspicion by span index (Debate)
         * o<N>     : Select option by index from debate menu
         * c        : Close menu (Debate)
         */

        std::string cmd;
        while (true) {
            std::cout << "\nCurrent Node: " << player.CurrentNodeId() << " (type=" << ToString(player.CurrentNode().type) << ")\n";
            DisplayView(player.View());
            std::cout << "\nEnter command (n/f/1/2/p<id>/#<N>/s<N>/o<N>/c/q): ";
            std::cin >> cmd;
            if (cmd == "q") break;

            if (cmd == "n") {
                player.Advance();
            } else if (cmd == "f") {
                player.FastForward();
            } else if (cmd == "1") {
                player.CommitOption("opt_agree");
            } else if (cmd == "2") {
                player.CommitOption("opt_rebut");
            } else if (cmd[0] == 'p' && cmd.size() > 1) {
                // p<itemId> - pick by itemId
                std::string itemId = cmd.substr(1);
                player.PickEvidence(itemId);
            } else if (cmd[0] == '#' && cmd.size() > 1) {
                // #<index> - pick by index from present view
                try {
                    size_t index = std::stoul(cmd.substr(1));
                    const auto& view = player.View();
                    if (view.present.has_value() && index < view.present->items.size()) {
                        const std::string& itemId = view.present->items[index].first;
                        std::cout << "Picking: " << itemId << "\n";
                        player.PickEvidence(itemId);
                    } else {
                        std::cout << "Invalid index or not in Present node\n";
                    }
                } catch (...) {
                    std::cout << "Invalid index format\n";
                }
            } else if (cmd[0] == 's' && cmd.size() > 1) {
                // s<index> - open suspicion by span index
                try {
                    size_t index = std::stoul(cmd.substr(1));
                    const auto& view = player.View();
                    if (view.debate.has_value() && index < view.debate->spanIds.size()) {
                        const std::string& spanId = view.debate->spanIds[index];
                        std::cout << "Opening suspicion: " << spanId << "\n";
                        player.OpenSuspicion(spanId);
                    } else {
                        std::cout << "Invalid span index or not in Debate node\n";
                    }
                } catch (...) {
                    std::cout << "Invalid span index format\n";
                }
            } else if (cmd[0] == 'o' && cmd.size() > 1) {
                // o<index> - select option from debate menu
                try {
                    size_t index = std::stoul(cmd.substr(1));
                    const auto& view = player.View();
                    if (view.debate.has_value() && view.debate->menuOpen && index < view.debate->options.size()) {
                        const std::string& optionId = view.debate->options[index].first;
                        std::cout << "Selecting option: " << optionId << "\n";
                        player.CommitOption(optionId);
                    } else {
                        std::cout << "Invalid option index or menu not open\n";
                    }
                } catch (...) {
                    std::cout << "Invalid option index format\n";
                }
            } else if (cmd == "c") {
                player.CloseDebateMenu();
            } else {
                std::cout << "Unknown command: " << cmd << "\n";
            }
        }
        
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}
