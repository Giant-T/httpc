cmake_minimum_required(VERSION 3.10)

project(httpc)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED True)

# Generate the `compile_commands.json` file. src: https://mzhang.io/posts/2022-03-03-clangd-in-nix/
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_C_STANDARD_INCLUDE_DIRECTORIES
    ${CMAKE_C_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

add_executable(httpc src/main.c include/print.h src/print.c include/request.h src/request.c include/response.h src/response.c include/server.h src/server.c)

target_include_directories(httpc PRIVATE include)
