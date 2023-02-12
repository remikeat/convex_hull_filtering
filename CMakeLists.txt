cmake_minimum_required(VERSION 3.15)
set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

FetchContent_Declare(
  json
  URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

project(ConvexHullFiltering)

file(GLOB_RECURSE sources src/convex_hull_filtering/*.cpp)
add_executable(convex_hull_filtering src/main.cpp ${sources})
target_include_directories(convex_hull_filtering PUBLIC include)
target_link_libraries(convex_hull_filtering PRIVATE nlohmann_json::nlohmann_json)
target_compile_options(convex_hull_filtering PRIVATE -Wall -Wextra -Wpedantic -Werror)

enable_testing()
file(GLOB_RECURSE test_sources test/*.cpp)
add_executable(convex_hull_filtering_test ${test_sources} ${sources})
target_include_directories(convex_hull_filtering_test PUBLIC include)
target_link_libraries(convex_hull_filtering_test GTest::gtest_main)
target_compile_options(convex_hull_filtering_test PRIVATE -Wall -Wextra -Wpedantic -Werror)

include(GoogleTest)
gtest_discover_tests(convex_hull_filtering_test)
