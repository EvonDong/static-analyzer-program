file(GLOB srcs "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pql/**/*.cpp")
file(GLOB headers "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/**/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/src/pql/**/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/src/pql/**/*.hpp")
add_library(spa ${srcs} ${headers})
# this makes the headers accessible for other projects which uses spa lib
target_include_directories(
        spa
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/adt
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/frontend
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pkb
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pql/evaluator
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pql/parser
        PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/pql/optimizer
)


