// Tests/Game/Story/StoryRuntimeTest.cpp
#include "Game/Story/StoryGraphLoader.h"
#include "Game/Story/StoryRuntime.h"
#include "Game/Common/Logger.h"

#include "Utils/DiskFileSystem.h"
#include <iostream>

using namespace Salt2D::Game::Story;

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    try {
        std::cout << "=== StoryRuntime Test ===\n\n";

        // 创建Logger
        auto logger = Salt2D::Game::MakeConsoleAndFileLogger("Logs/story_runtime_test.log", Salt2D::Game::LogLevel::Info, Salt2D::Game::LogLevel::Debug);

        // 使用DiskFileSystem
        Salt2D::Utils::DiskFileSystem diskFS;

        // 构建图路径
        std::filesystem::path graphPath = std::filesystem::path("Assets/Story/Demo/demo_story.graph.json");

        // 加载图
        StoryGraphLoadOptions loadOpt;
        StoryGraph graph = LoadStoryGraph(diskFS, graphPath, loadOpt);
        std::cout << "Loaded story graph with " << graph.Nodes().size() << " nodes and " << graph.Edges().size() << " edges.\n\n";

        // 创建运行时
        StoryRuntime runtime(graph);
        runtime.SetLogger(&logger);  // 设置Logger

        // 设置效果回调
        runtime.SetEffectCallback([](const Effect& effect) {
            std::cout << "[Effect Callback] Effect applied: type='" << effect.type << "', name='" << effect.name << "'\n";
        });

        // 启动运行时
        runtime.Start("n0_intro");

        // 推送一些事件
        runtime.PushEvent({Trigger::Auto, ""});                 // n0 -> n11 choice

        runtime.PushEvent({Trigger::Option, "opt_badend"});     // n11 -> be_choice_badend
        runtime.PushEvent({Trigger::Auto, ""});                 // be_choice_badend -> be_choice_back
        runtime.PushEvent({Trigger::Option, "opt_back"});       // be_choice_back -> n11 choice

        runtime.PushEvent({Trigger::Option, "opt_continue"});   // n11 -> n1

        runtime.PushEvent({Trigger::Option, "opt_agree"});      // n1 -> n4
        runtime.PushEvent({Trigger::Auto, ""});                 // n4 -> n1

        runtime.PushEvent({Trigger::Option, "opt_rebut"});      // n1 -> n3 (cue: rebuttal)
        runtime.PushEvent({Trigger::Auto, ""});                 // n3 -> n6

        runtime.PushEvent({Trigger::Pick, "evid_pen"});         // n6 -> n8
        runtime.PushEvent({Trigger::Auto, ""});                 // n8 -> n6

        runtime.PushEvent({Trigger::Pick, "evid_apple"});       // n6 -> n7 (cue: rebuttal)
        runtime.PushEvent({Trigger::Auto, ""});                 // n7 -> n10
        
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << "\n";
        return 1;
    }
}