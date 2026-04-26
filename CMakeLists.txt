cmake_minimum_required(VERSION 3.20)

# ── Toolchain must be set BEFORE project() ────────────────────────────────────
if(NOT CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/toolchain-arm-none-eabi.cmake"
        CACHE FILEPATH "Toolchain file")
endif()

project(stm32g071-pilot C CXX ASM)

set(CMAKE_C_STANDARD    11)
set(CMAKE_C_EXTENSIONS  ON)  # GNU extensions needed for embedded idioms
set(CMAKE_CXX_STANDARD  17)
set(CMAKE_CXX_EXTENSIONS ON)

# ── Target name ───────────────────────────────────────────────────────────────
set(TARGET_NAME ${PROJECT_NAME})

# ── Source files ──────────────────────────────────────────────────────────────
set(APP_SOURCES
    Core/Src/main.cpp
    Core/Src/system_stm32g0xx.c
    Core/Src/startup_stm32g071xx.s
    Core/Src/uart.c
    Core/Src/syscalls.c
)

# ── Include directories ───────────────────────────────────────────────────────
set(INCLUDE_DIRS
    Core/Inc
    thirdparty/CMSIS/Core/Include
    thirdparty/CMSIS/Device/ST/STM32G0xx/Include
)

# ── Compile definitions ───────────────────────────────────────────────────────
set(COMPILE_DEFS
    STM32G071xx
    USE_FULL_LL_DRIVER
)

# ── Common compile flags ──────────────────────────────────────────────────────
set(COMMON_FLAGS
    -Wall
    -Wextra
    -Wpedantic
    -fdata-sections
    -ffunction-sections
    -fno-exceptions
    -fno-unwind-tables
    -fno-asynchronous-unwind-tables
)

set(CXX_ONLY_FLAGS
    -fno-rtti
    -fno-use-cxa-atexit
    -fno-threadsafe-statics
)

# ── Linker script ─────────────────────────────────────────────────────────────
set(LINKER_SCRIPT
    "${CMAKE_CURRENT_SOURCE_DIR}/ld/stm32g071rb_flash.ld")

# ── Linker flags ──────────────────────────────────────────────────────────────
set(LINKER_FLAGS
    -T${LINKER_SCRIPT}
    -Wl,--gc-sections
    -Wl,-Map=${TARGET_NAME}.map
    -Wl,--print-memory-usage
    --specs=nosys.specs
    --specs=nano.specs
)

# ── ELF target ────────────────────────────────────────────────────────────────
add_executable(${TARGET_NAME}.elf ${APP_SOURCES})

target_include_directories(${TARGET_NAME}.elf PRIVATE ${INCLUDE_DIRS})
target_compile_definitions(${TARGET_NAME}.elf PRIVATE ${COMPILE_DEFS})
target_compile_options    (${TARGET_NAME}.elf PRIVATE
    ${COMMON_FLAGS}
    $<$<COMPILE_LANGUAGE:CXX>:${CXX_ONLY_FLAGS}>
)

target_link_options(${TARGET_NAME}.elf PRIVATE ${LINKER_FLAGS})

# Ensure linker script changes trigger a relink
set_target_properties(${TARGET_NAME}.elf PROPERTIES
    LINK_DEPENDS ${LINKER_SCRIPT})

# ── Post-build: HEX + BIN + size ─────────────────────────────────────────────
add_custom_command(TARGET ${TARGET_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex   $<TARGET_FILE:${TARGET_NAME}.elf>
            ${TARGET_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET_NAME}.elf>
            ${TARGET_NAME}.bin
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${TARGET_NAME}.elf>
    COMMENT "Generating HEX and BIN artifacts"
)

# ── Flash target (st-flash) ───────────────────────────────────────────────────
find_program(ST_FLASH st-flash)
if(ST_FLASH)
    add_custom_target(flash
        COMMAND ${ST_FLASH} write ${TARGET_NAME}.bin 0x08000000
        DEPENDS ${TARGET_NAME}.elf
        COMMENT "Flashing via st-flash"
    )
endif()

# ── Flash target (OpenOCD) ────────────────────────────────────────────────────
find_program(OPENOCD openocd)
if(OPENOCD)
    add_custom_target(flash-openocd
        COMMAND ${OPENOCD}
            -f interface/stlink.cfg
            -f target/stm32g0x.cfg
            -c "program ${TARGET_NAME}.elf verify reset exit"
        DEPENDS ${TARGET_NAME}.elf
        COMMENT "Flashing via OpenOCD"
    )

    add_custom_target(debug-openocd
        COMMAND ${OPENOCD}
            -f interface/stlink.cfg
            -f target/stm32g0x.cfg
        COMMENT "Starting OpenOCD GDB server (port 3333)"
    )
endif()

# ── Erase target ──────────────────────────────────────────────────────────────
if(ST_FLASH)
    add_custom_target(erase
        COMMAND ${ST_FLASH} erase
        COMMENT "Erasing target flash"
    )
endif()
