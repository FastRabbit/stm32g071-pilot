# cmake/toolchain-arm-none-eabi.cmake
# Cross-compilation toolchain for Cortex-M0+ (STM32G071)

set(CMAKE_SYSTEM_NAME      Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# ── Toolchain executables ─────────────────────────────────────────────────────
find_program(ARM_CC  arm-none-eabi-gcc  REQUIRED)
find_program(ARM_CXX arm-none-eabi-g++  REQUIRED)
find_program(ARM_ASM arm-none-eabi-gcc  REQUIRED)  # gcc front-end for .s
find_program(ARM_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(ARM_SIZE    arm-none-eabi-size    REQUIRED)

set(CMAKE_C_COMPILER   ${ARM_CC}  CACHE FILEPATH "" FORCE)
set(CMAKE_CXX_COMPILER ${ARM_CXX} CACHE FILEPATH "" FORCE)
set(CMAKE_ASM_COMPILER ${ARM_ASM} CACHE FILEPATH "" FORCE)

# ── Prevent CMake from trying to link a test program during configure ─────────
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# ── CPU / FPU flags ───────────────────────────────────────────────────────────
set(CPU_FLAGS
    -mcpu=cortex-m0plus
    -mthumb
    -mfloat-abi=soft
)

string(JOIN " " CPU_FLAGS_STR ${CPU_FLAGS})
set(CMAKE_C_FLAGS_INIT   "${CPU_FLAGS_STR}")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS_STR} -fno-exceptions -fno-rtti -fno-use-cxa-atexit -fno-threadsafe-statics")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS_STR} -x assembler-with-cpp")
set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS_STR}")

# ── Store tool paths for post-build steps ────────────────────────────────────
set(CMAKE_OBJCOPY ${ARM_OBJCOPY} CACHE FILEPATH "" FORCE)
set(CMAKE_SIZE    ${ARM_SIZE}    CACHE FILEPATH "" FORCE)
