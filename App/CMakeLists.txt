set(APP_SOURCES
    main.cpp
)

add_executable(manosaba_plus WIN32
    ${APP_SOURCES}
)

target_include_directories(manosaba_plus PRIVATE
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(manosaba_plus PRIVATE
    Core
    Render
    RHI_DX11
    user32
    gdi32
)

target_compile_definitions(manosaba_plus PRIVATE UNICODE _UNICODE)

if(MSVC)
    target_compile_options(manosaba_plus PRIVATE /EHsc)
endif()

add_custom_command(TARGET manosaba_plus POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/Assets
        $<TARGET_FILE_DIR:manosaba_plus>/Assets
    COMMENT "Copying assets to exe directory"
)
