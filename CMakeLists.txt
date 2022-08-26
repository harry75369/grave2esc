cmake_minimum_required(VERSION 3.0)

project(grave2esc)

add_executable(grave2esc grave2esc.c)
target_compile_options(grave2esc PRIVATE -Wall -Wextra)

install(TARGETS grave2esc RUNTIME DESTINATION bin)
