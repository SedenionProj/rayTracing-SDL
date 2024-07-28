cmake_minimum_required(VERSION 3.7)

set(PROJ_NAME "rayTracer")
set(EXE_NAME ${PROJ_NAME})

project(${PROJ_NAME})

if (MSVC)
  add_definitions( /wd4018 /wd4100 /wd4101 /wd4127 /wd4146 /wd4232 /wd4242 /wd4244 /wd4245 /wd4267 /wd4305 /wd4309 
    /wd4310 /wd4334 /wd4456 /wd4464 /wd4668 /wd4701 /wd4703 /wd4711 /wd4756 /wd4820 /wd5045 /wd5250)
endif ()

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
target_include_directories(${EXE_NAME} PRIVATE ${PROJ_NAME}/vendor/stb)

target_include_directories(${EXE_NAME} PRIVATE ${PROJ_NAME}/vendor/SDL/include)
target_link_directories(${EXE_NAME} PRIVATE ${PROJ_NAME}/vendor/SDL/lib/x64)
target_link_libraries(${EXE_NAME} SDL2 SDL2main)