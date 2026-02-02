// Tests/Game/Story/StoryPlayerTest.cpp
#include "Game/Story/StoryGraphLoader.h"
#include "Game/Story/StoryPlayer.h"
#include "Utils/DiskFileSystem.h"
#include <iostream>
#include <windows.h>

using namespace Salt2D::Game::Story;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    SetConsoleOutputCP(CP_UTF8);
    try {
        Salt2D::Utils::DiskFileSystem diskFS;
        auto graphPath = std::filesystem::path("Assets/Story/Demo/demo_story.graph.json");
        auto graph = LoadStoryGraph(diskFS, graphPath);
        std::cout << "Loaded story graph with " << graph.Nodes().size() << " nodes and " << graph.Edges().size() << " edges.\n\n";

        auto logger = Salt2D::Game::MakeConsoleLogger(Salt2D::Game::LogLevel::Debug);

        StoryPlayer player(graph, diskFS);
        player.SetEffectCallback([](const Effect& effect) {
            std::cout << "[Effect Callback] Effect applied: type='" << effect.type << "', name='" << effect.name << "'\n";
        });

        player.SetLogger(&logger);
        player.Start("n0_intro");

        /* ======
         * n     : Advance
         * f     : FastForward
         * 1     : Commit option "opt_agree"
         * 2     : Commit option "opt_rebut"
         * P     : Pick evidence "evid_pen"
         * A     : Pick evidence "evid_apple"
         */

        char cmd = '\0';
        while (true) {
            std::cout << "\nCurrent Node: " << player.CurrentNodeId() << " (type=" << ToString(player.CurrentNode().type) << ")\n";
            std::cout << "Enter command (n=Advance, f=FastForward, 1=opt_agree, 2=opt_rebut, P=evid_pen, A=evid_apple, q=quit): ";
            std::cin >> cmd;
            if (cmd == 'q') break;

            switch (cmd) {
                case 'n':
                    player.Advance();
                    break;
                case 'f':
                    player.FastForward();
                    break;
                case '1':
                    player.CommitOption("opt_agree");
                    break;
                case '2':
                    player.CommitOption("opt_rebut");
                    break;
                case 'P':
                    player.PickEvidence("evid_pen");
                    break;
                case 'A':
                    player.PickEvidence("evid_apple");
                    break;
                default:
                    std::cout << "Unknown command: " << cmd << "\n";
                    break;
            }
        }
        
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}