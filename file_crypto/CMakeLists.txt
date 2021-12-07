cmake_minimum_required(VERSION 3.21)
project(file_crypto C)

set(CMAKE_C_STANDARD 99)

add_executable(file_crypto main.c aes128_crypt.c aes128_block_crypt.c)
