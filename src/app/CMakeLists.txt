target_sources(
    life
    PRIVATE
    Application.cpp

    # WINDOW
    Window/Window.hpp
    Window/Window.cpp
    Window/Event.hpp
    Window/Event.cpp
)

target_include_directories(life PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_precompile_headers(life PRIVATE pch.hpp pch.cpp)

target_link_libraries(life PRIVATE glad)
target_link_libraries(life PRIVATE glfw)
target_link_libraries(life PRIVATE lwvl)
