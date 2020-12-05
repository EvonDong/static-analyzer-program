file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
add_executable(integration_testing ${srcs})


target_link_libraries(integration_testing spa)

file(GLOB TEXT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/SIMPLESourceFiles/*.txt")
file(COPY ${TEXT_FILES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/SIMPLESourceFiles")