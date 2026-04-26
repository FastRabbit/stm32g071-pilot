/**
 * @file syscalls.c
 * @brief Newlib syscall stubs — redirects stdout/stderr to USART2.
 *
 * The C/C++ runtime calls _write() for every printf/puts/fwrite to
 * stdout (fd=1) or stderr (fd=2).  We forward those bytes to USART2.
 *
 * _read, _close, _lseek, _fstat, _isatty are stubbed to silence linker
 * warnings from nosys.specs and to satisfy newlib's reentrancy layer.
 */
#include "uart.h"
#include <sys/stat.h>
#include <errno.h>
#include <stdint.h>

/* Suppress "implicit declaration" warnings for newlib internals */
#undef errno
extern int errno;

/* Provided by the linker script */
extern char end;        /* first byte after BSS/heap base */
extern char __StackTop; /* top of RAM (stack grows down from here) */

/* =========================================================== */
/*  _sbrk — minimal heap allocator for newlib                  */
/* =========================================================== */
void *_sbrk(int incr)
{
    static char *heap_end = &end;
    char *prev = heap_end;

    if ((heap_end + incr) > &__StackTop) {
        errno = ENOMEM;
        return (void *)-1;
    }
    heap_end += incr;
    return (void *)prev;
}

/* =========================================================== */
/*  _write — stdout/stderr → USART2                             */
/* =========================================================== */
int _write(int fd, const char *buf, int len)
{
    (void)fd;   /* treat all fds the same: route to USART2 */
    UART2_SendBuffer((const uint8_t *)buf, (uint32_t)len);
    return len;
}

/* =========================================================== */
/*  Minimal stubs required by newlib                            */
/* =========================================================== */
int _read(int fd, char *buf, int len)
{
    (void)fd; (void)buf; (void)len;
    errno = ENOSYS;
    return -1;
}

int _close(int fd)
{
    (void)fd;
    return -1;
}

int _lseek(int fd, int offset, int whence)
{
    (void)fd; (void)offset; (void)whence;
    return -1;
}

int _fstat(int fd, struct stat *st)
{
    (void)fd;
    st->st_mode = S_IFCHR;
    return 0;
}

int _isatty(int fd)
{
    (void)fd;
    return 1;
}
