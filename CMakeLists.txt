cmake_minimum_required(VERSION 4.4)
project(app 
	VERSION 0.1.0
	DESCRIPTION "software rasteriser project" 
	HOMEPAGE_URL "https://github.com/brianuy-me/software-rasteriser" 
	LANGUAGES C
)

include(GNUInstallDirs)
include(FetchContent)

FetchContent_Declare(SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.16
    GIT_SHALLOW TRUE
    EXCLUDE_FROM_ALL
    FIND_PACKAGE_ARGS CONFIG
)
FetchContent_MakeAvailable(SDL3)

add_executable(app)

target_sources(app PRIVATE
    src/main.c
)

target_include_directories(app PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_compile_features(app PRIVATE c_std_17)
target_compile_options(app PRIVATE $<$<C_COMPILER_ID:MSVC>:/W4 /utf-8>)
set_target_properties(app PROPERTIES C_EXTENSIONS OFF)

target_link_libraries(app PRIVATE SDL3::SDL3)

add_custom_command(TARGET app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            -t $<TARGET_FILE_DIR:app> $<TARGET_RUNTIME_DLLS:app>
    COMMAND_EXPAND_LISTS)


install(TARGETS app RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES $<TARGET_RUNTIME_DLLS:app> DESTINATION ${CMAKE_INSTALL_BINDIR})