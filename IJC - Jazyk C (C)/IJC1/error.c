#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include <stdarg.h>
#include <string.h>


void warning_msg(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "CHYBA: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
}

void fatal_error(const char *fmt, ...){
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "CHYBA: ");
	vfprintf(stderr, fmt, args);
	va_end(args);
	exit(1);
}

