# All the source files for the bot.
file(GLOB BOT_SOURCES "*.cpp" "*.h" "*.hpp")

include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/lib/cpp-sc2/include
    ${PROJECT_SOURCE_DIR}/lib/cpp-sc2/contrib/protobuf/src
    ${PROJECT_BINARY_DIR}/lib/cpp-sc2/generated
    ${PROJECT_SOURCE_DIR}/lib/json/include
)

link_directories(${PROJECT_BINARY_DIR}/cpp-sc2/bin)

# Enable compilation of the SC2 version of the bot.
add_definitions(-DSC2API)

# Show more warnings at compiletime.
if (MSVC)
    # FIXME: put Windows specific options here.
else ()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif ()

# Create the executable.
add_executable(CommandCenter ${BOT_SOURCES})
target_link_libraries(CommandCenter
    sc2api sc2lib sc2utils sc2protocol libprotobuf
)

if (APPLE)
    target_link_libraries(CommandCenter "-framework Carbon")
endif ()

# Linux specific.
if (UNIX AND NOT APPLE)
    target_link_libraries(CommandCenter pthread dl)
endif ()
