cmake_minimum_required(VERSION 3.21)
project(aes128 C)

set(CMAKE_C_STANDARD 99)

add_executable(aes128 main.c aes128_blcok_crypt.c aes128_crypt.c aes128_crypt.h aes_struct.h)
