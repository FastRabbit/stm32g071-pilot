cmake_minimum_required(VERSION 3.20)
project(stm32g071-pilot-tests C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

enable_testing()

add_executable(fdcan_codec_tests
  fdcan_codec_tests.c
  ../Driver/Src/fdcan_codec.c
)

target_include_directories(fdcan_codec_tests
  PRIVATE
    ../Driver/Inc
)

add_test(
  NAME fdcan_codec_tests
  COMMAND fdcan_codec_tests
)
