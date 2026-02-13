set(GAME_SOURCES
    Common/Logger.cpp
    Director/HudEvaluator.cpp
    Director/StageWorld.cpp
    Director/StageCameraDirector.cpp
    Story/StoryGraphLoader.cpp
    Story/StoryPlayer.cpp
    Story/StoryRuntime.cpp
    Story/Resources/VnScript.cpp
    Story/Resources/PresentDefLoader.cpp
    Story/Resources/DebateDefLoader.cpp
    Story/Resources/ChoiceDefLoader.cpp
    Story/Resources/PerformanceDefLoader.cpp
    Story/Resources/CastDefLoader.cpp
    Story/Resources/StageDefLoader.cpp
    Story/Runners/VnRunner.cpp
    Story/Runners/PresentRunner.cpp
    Story/Runners/DebateRunner.cpp
    Story/Runners/ChoiceRunner.cpp
    Story/TextMarkup/SusMarkup.cpp
    UI/Framework/UIBaker.cpp
    UI/Framework/UIEmitter.cpp
    UI/Framework/UIInteraction.cpp
    UI/Widgets/VnDialogWidget.cpp
    UI/Widgets/VnAutoWidget.cpp
    UI/Widgets/ChoiceDialogWidget.cpp
    UI/Widgets/PresentDialogWidget.cpp
    UI/Widgets/DebateDialogWidget.cpp
    UI/Widgets/DebateMenuWidget.cpp
    UI/Widgets/DebateSpeedWidget.cpp
    UI/Widgets/UIButtonWidget.cpp
    UI/Widgets/TimerWidget.cpp
    RenderBridge/TextService.cpp
    RenderBridge/TextureService.cpp
    RenderBridge/TextureCatalog.cpp
    Screens/VnScreen.cpp
    Screens/DebateScreen.cpp
    Screens/PresentScreen.cpp
    Screens/ChoiceScreen.cpp
    Screens/StoryOverlayLayer.cpp
    Session/StoryScreenManager.cpp
    Session/StorySession.cpp
    Session/StoryActionMap.cpp
    Session/StoryHistory.cpp
)

add_library(Game STATIC
    ${GAME_SOURCES}
)

target_include_directories(Game PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_include_directories(Game PRIVATE
    ${CMAKE_SOURCE_DIR}/ThirdParty
)

target_link_libraries(Game PRIVATE
    Utils
)