#include "Game/Flow/GameFlow.h"
#include "Core/Time/FrameClock.h"
#include "Utils/DiskFileSystem.h"
#include <iostream>
#include <windows.h>

using namespace Salt2D::Game::Flow;
using namespace Salt2D::Game::Session;
using namespace Salt2D::Game::Story;
using namespace Salt2D::Utils;
using namespace Salt2D::Core;

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

static std::string FlowStateToString(FlowState state) {
    switch (state) {
        case FlowState::Idle: return "Idle";
        case FlowState::InChapter: return "InChapter";
        case FlowState::Finished: return "Finished";
        case FlowState::GateReady: return "GateReady";
        default: return "Unknown";
    }
}

static std::string ChapterKindToString(ChapterKind kind) {
    switch (kind) {
        case ChapterKind::Novel: return "Novel";
        case ChapterKind::Trial: return "Trial";
        default: return "Unknown";
    }
}

static std::string FlowEventToString(FlowEvent event) {
    switch (event) {
        case FlowEvent::None: return "None";
        case FlowEvent::SessionCleared: return "SessionCleared";
        case FlowEvent::SessionCreated: return "SessionCreated";
        case FlowEvent::Finished: return "Finished";
        default: return "Unknown";
    }
}

void DisplayFlowStatus(GameFlow& flow) {
    std::cout << "\n========== Game Flow Status ==========\n";
    std::cout << "State: " << FlowStateToString(flow.State()) << "\n";
    std::cout << "Chapter Index: " << flow.ChapterIndex() << "\n";
    
    const auto* chapter = flow.CurrentChapter();
    if (chapter) {
        std::cout << "Current Chapter:\n";
        std::cout << "  ID: " << chapter->chapterId << "\n";
        std::cout << "  Kind: " << ChapterKindToString(chapter->kind) << "\n";
        std::cout << "  Story Root: " << chapter->storyRoot.string() << "\n";
        std::cout << "  Start Node: " << chapter->startNode << "\n";
    } else {
        std::cout << "Current Chapter: None\n";
    }
    
    std::cout << "Has Session: " << (flow.HasSession() ? "Yes" : "No") << "\n";
    std::cout << "======================================\n";
}

void DisplayStoryView(const StoryView& view) {
    std::cout << "\n--- Story View ---\n";
    std::cout << "Node Type: " << ToString(view.nodeType) << "\n";
    
    if (view.vn.has_value()) {
        const auto& vn = *view.vn;
        std::cout << "Speaker: " << vn.speaker << "\n";
        std::cout << "Full Text: " << vn.fullText << "\n";
        auto currentText = Utf8PrefixByCodepoints(vn.fullText, vn.revealed);
        auto totalCodepoints = std::count_if(vn.fullText.begin(), vn.fullText.end(), [](unsigned char c) {
            return (c & 0xC0) != 0x80;
        });
        std::cout << "Revealed: " << currentText << " [" << vn.revealed << "/" << totalCodepoints << "]\n";
        std::cout << "Line Done: " << (vn.lineDone ? "Yes" : "No") << "\n";
        std::cout << "Finished: " << (vn.finished ? "Yes" : "No") << "\n";
    }
    
    if (view.present.has_value()) {
        const auto& present = *view.present;
        std::cout << "Prompt: " << present.prompt << "\n";
        std::cout << "Available Items:\n";
        for (size_t i = 0; i < present.items.size(); ++i) {
            std::cout << "  [" << i << "] " << present.items[i].first << " - " << present.items[i].second << "\n";
        }
    }
    
    if (view.debate.has_value()) {
        const auto& debate = *view.debate;
        std::cout << "Statement: " << debate.statementIndex << "/" << (debate.statementCount - 1) << "\n";
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

    if (view.choice.has_value()) {
        const auto& choice = *view.choice;
        std::cout << "Available Options:\n";
        for (size_t i = 0; i < choice.options.size(); ++i) {
            std::cout << "  [" << i << "] " << choice.options[i].first << " - " << choice.options[i].second << "\n";
        }
    }
    std::cout << "------------------\n";
}

void DisplayHelp() {
    std::cout << "\n========== Commands ==========\n";
    std::cout << "Flow Commands:\n";
    std::cout << "  start      - Start the game flow\n";
    std::cout << "  gate       - Confirm gate (when in GateReady state)\n";
    std::cout << "  tick       - Advance time (tick the flow)\n";
    std::cout << "  status     - Show current flow status\n";
    std::cout << "\nStory Commands (when in InChapter):\n";
    std::cout << "  n          - Advance story / Advance statement (Debate)\n";
    std::cout << "  f          - FastForward\n";
    std::cout << "  p<id>      - Pick evidence by itemId (e.g., 'pevid_pen')\n";
    std::cout << "  #<N>       - Pick evidence by index from Present view\n";
    std::cout << "  s<N>       - Open suspicion by span index (Debate)\n";
    std::cout << "  o<N>       - Select option by index\n";
    std::cout << "  c          - Close debate menu\n";
    std::cout << "\nGeneral:\n";
    std::cout << "  h, help    - Show this help\n";
    std::cout << "  q, quit    - Quit\n";
    std::cout << "==============================\n";
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    try {
        std::vector<ChapterDef> chapters = {
            {"demo_novel", ChapterKind::Novel, "Assets/Story/DemoNovel/", "demo_novel.graph.json", "n0_intro", ""},
            {"demo_trial", ChapterKind::Trial, "Assets/Story/DemoTrial/", "demo_trial.graph.json", "n0_intro", "inquisition"},
        };

        DiskFileSystem diskFS;
        GameFlow flow(diskFS);
        flow.SetChapters(std::move(chapters));

        std::cout << "GameFlow Test Program\n";
        std::cout << "Type 'help' for available commands\n";

        DisplayFlowStatus(flow);
        DisplayHelp();

        std::string cmd;
        FrameTime frameTime;
        frameTime.dtSec = 0.016f;  // Simulate ~60fps

        while (true) {
            std::cout << "\nEnter command: ";
            std::cin >> cmd;

            if (cmd == "q" || cmd == "quit") {
                break;
            } else if (cmd == "h" || cmd == "help") {
                DisplayHelp();
            } else if (cmd == "start") {
                std::cout << "Starting game flow...\n";
                flow.Start();
                DisplayFlowStatus(flow);
            } else if (cmd == "gate") {
                if (flow.State() == FlowState::GateReady) {
                    std::cout << "Confirming gate...\n";
                    flow.ConfirmGate();
                    DisplayFlowStatus(flow);
                } else {
                    std::cout << "Not in GateReady state. Current state: " << FlowStateToString(flow.State()) << "\n";
                }
            } else if (cmd == "tick") {
                flow.Tick(frameTime);
                if (auto event = flow.ConsumeEvent(); event.has_value()) {
                    std::cout << "[Flow Event: " << FlowEventToString(*event) << "]\n";
                    DisplayFlowStatus(flow);
                }
            } else if (cmd == "status") {
                DisplayFlowStatus(flow);
                if (flow.HasSession() && flow.Player()) {
                    auto* player = flow.Player();
                    std::cout << "Current Node: " << player->CurrentNodeId() 
                              << " (type=" << ToString(player->CurrentNode().type) << ")\n";
                    DisplayStoryView(player->View());
                }
            } else {
                // Story player commands
                if (flow.State() != FlowState::InChapter || !flow.HasSession()) {
                    std::cout << "Not in chapter or no session. Current state: " << FlowStateToString(flow.State()) << "\n";
                    continue;
                }

                auto* player = flow.Player();
                if (!player) {
                    std::cout << "Player not available\n";
                    continue;
                }

                if (cmd == "n") {
                    player->Advance();
                    DisplayStoryView(player->View());
                } else if (cmd == "f") {
                    player->FastForward();
                    DisplayStoryView(player->View());
                } else if (cmd[0] == 'p' && cmd.size() > 1) {
                    std::string itemId = cmd.substr(1);
                    player->PickEvidence(itemId);
                    DisplayStoryView(player->View());
                } else if (cmd[0] == '#' && cmd.size() > 1) {
                    try {
                        size_t index = std::stoul(cmd.substr(1));
                        const auto& view = player->View();
                        if (view.present.has_value() && index < view.present->items.size()) {
                            const std::string& itemId = view.present->items[index].first;
                            std::cout << "Picking: " << itemId << "\n";
                            player->PickEvidence(itemId);
                            DisplayStoryView(player->View());
                        } else {
                            std::cout << "Invalid index or not in Present node\n";
                        }
                    } catch (...) {
                        std::cout << "Invalid index format\n";
                    }
                } else if (cmd[0] == 's' && cmd.size() > 1) {
                    try {
                        size_t index = std::stoul(cmd.substr(1));
                        const auto& view = player->View();
                        if (view.debate.has_value() && index < view.debate->spanIds.size()) {
                            const std::string& spanId = view.debate->spanIds[index];
                            std::cout << "Opening suspicion: " << spanId << "\n";
                            player->OpenSuspicion(spanId);
                            DisplayStoryView(player->View());
                        } else {
                            std::cout << "Invalid span index or not in Debate node\n";
                        }
                    } catch (...) {
                        std::cout << "Invalid span index format\n";
                    }
                } else if (cmd[0] == 'o' && cmd.size() > 1) {
                    try {
                        size_t index = std::stoul(cmd.substr(1));
                        const auto& view = player->View();
                        if (view.debate.has_value() && view.debate->menuOpen && index < view.debate->options.size()) {
                            const std::string& optionId = view.debate->options[index].first;
                            std::cout << "Selecting option: " << optionId << "\n";
                            player->CommitOption(optionId);
                            DisplayStoryView(player->View());
                        } else if (view.choice.has_value() && index < view.choice->options.size()) {
                            const std::string& optionId = view.choice->options[index].first;
                            std::cout << "Selecting choice option: " << optionId << "\n";
                            player->CommitOption(optionId);
                            DisplayStoryView(player->View());
                        } else {
                            std::cout << "Invalid option index or menu not open\n";
                        }
                    } catch (...) {
                        std::cout << "Invalid option index format\n";
                    }
                } else if (cmd == "c") {
                    player->CloseDebateMenu();
                    DisplayStoryView(player->View());
                } else {
                    std::cout << "Unknown command: " << cmd << ". Type 'help' for available commands.\n";
                }
            }

            // Auto-tick after each command when in InChapter
            if (flow.State() == FlowState::InChapter) {
                flow.Tick(frameTime);
            }
        }

        std::cout << "\nTest completed.\n";

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }

    return 0;
}