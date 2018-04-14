#include <core/eos_internal.h>

void _os_serial_puts(const char *s) {
	while (*s) {
		putchar(*s++);
	}
}
