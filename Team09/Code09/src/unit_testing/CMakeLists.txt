file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(unit_testing ${srcs})
set(CMAKE_VERBOSE_MAKEFILE ON)

target_link_libraries(unit_testing spa)

# Add source .txt files for LexerTest
# If files are updated, make sure to re-run CMake to reflect changes
file(GLOB TEXT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/LexerTestFiles/*.txt")
file(COPY ${TEXT_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/LexerTestFiles")

file(GLOB TEXT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/frontend/ParserTestFiles/*.txt")
file(COPY ${TEXT_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/ParserTestFiles")
