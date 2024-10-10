include_directories(SYSTEM
    ${PROJECT_SOURCE_DIR}/lib/cpp-sc2/include
    ${PROJECT_SOURCE_DIR}/lib/cpp-sc2/contrib/protobuf/src
    ${PROJECT_BINARY_DIR}/lib/cpp-sc2/generated
    ${PROJECT_SOURCE_DIR}/lib/json/include
)

# All the source files for the bot.
file(GLOB BOT_SOURCES "../src/*.cpp" "../src/*.h" "../src/*.hpp")

file(GLOB LIBRARY_SOURCES "*.cpp" "*.h")

link_directories(${PROJECT_BINARY_DIR}/cpp-sc2/bin)

# Enable compilation of the SC2 version of the bot code
# TODO: Remove all remaining BW code
add_definitions(-DSC2API)

# Create the executable.
pybind11_add_module(library library.cpp library.h ${BOT_SOURCES} ${LIBRARY_SOURCES})
target_link_libraries(library PRIVATE
    sc2api sc2lib sc2utils sc2protocol libprotobuf
)
