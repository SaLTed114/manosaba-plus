set(GAME_SOURCES
    Common/Logger.cpp
    Story/StoryGraphLoader.cpp
    Story/StoryPlayer.cpp
    Story/StoryRuntime.cpp
    Story/Resources/VnScript.cpp
    Story/Resources/PresentDefLoader.cpp
    Story/Resources/DebateDefLoader.cpp
    Story/Runners/VnRunner.cpp
    Story/Runners/PresentRunner.cpp
    Story/Runners/DebateRunner.cpp
    UI/VnHud.cpp
    UI/PresentHud.cpp
    UI/DebateHud.cpp
    RenderBridge/TextService.cpp
    Screens/VnScreen.cpp
    Screens/DebateScreen.cpp
    Screens/PresentScreen.cpp
    Session/StoryScreenManager.cpp
    Session/StorySession.cpp
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