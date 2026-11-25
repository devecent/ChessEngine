cmake_minimum_required(VERSION 3.20)
project(ChessEngine LANGUAGES CXX)

add_library(chessengine_core STATIC
    src/board.cpp
    src/evaluation.cpp
    src/move.cpp
    src/movegen.cpp
    src/openingbook.cpp
    src/search.cpp
    src/transpositiontable.cpp
    src/uci.cpp
    src/zobrist.cpp
)
target_include_directories(chessengine_core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_compile_features(chessengine_core PUBLIC cxx_std_20)
set_target_properties(chessengine_core PROPERTIES CXX_EXTENSIONS OFF)

add_executable(devecent src/main.cpp)
target_link_libraries(devecent PRIVATE chessengine_core)
set_target_properties(devecent PROPERTIES CXX_EXTENSIONS OFF)

# The engine loads this book relative to its working directory.
set(OPENING_BOOK "${CMAKE_CURRENT_SOURCE_DIR}/openingbook/komodo.bin")
if(EXISTS "${OPENING_BOOK}")
    add_custom_command(TARGET devecent POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:devecent>/openingbook"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${OPENING_BOOK}" "$<TARGET_FILE_DIR:devecent>/openingbook/komodo.bin"
        VERBATIM
    )
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(chessengine_regression tests/regression.cpp)
    target_link_libraries(chessengine_regression PRIVATE chessengine_core)
    set_target_properties(chessengine_regression PROPERTIES CXX_EXTENSIONS OFF)
    add_test(NAME chessengine_regression COMMAND chessengine_regression)
    set_tests_properties(chessengine_regression PROPERTIES TIMEOUT 60)
endif()

foreach(target IN ITEMS chessengine_core devecent chessengine_regression)
    if(TARGET ${target})
        if(MSVC)
            target_compile_options(${target} PRIVATE /W4 /permissive-)
        elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
        endif()
    endif()
endforeach()
