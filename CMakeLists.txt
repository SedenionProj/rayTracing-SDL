cmake_minimum_required(VERSION 3.7)

set(PROJ_NAME "rayTracer")
set(EXE_NAME ${PROJ_NAME})

project(${PROJ_NAME})


# Add source files
file(GLOB_RECURSE SOURCES
    ${PROJ_NAME}/src/*.h
    ${PROJ_NAME}/src/*.cpp
)
set(SDL2_INCLUDE_DIRS ${PROJ_NAME}/vendor/SDL/include)


# Add executable
add_executable(${EXE_NAME} ${SOURCES})
target_compile_features(${EXE_NAME} PUBLIC cxx_std_20)

target_include_directories(${EXE_NAME} PRIVATE ${PROJ_NAME}/vendor/glm)

target_include_directories(${EXE_NAME} PRIVATE ${PROJ_NAME}/vendor/SDL/include)
target_link_directories(${EXE_NAME} PRIVATE ${PROJ_NAME}/vendor/SDL/lib/x64)
target_link_libraries(${EXE_NAME} SDL2 SDL2main)