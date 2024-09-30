#pragma once

#include <stdarg.h>
#include <stdlib.h>
#include "FlexibleCircularBuffer.h"

struct LogLine : public BufferLine<char>
{
};

int log_toBuffer(const char *fmt, va_list list);

LogLine *log_readFirst();

LogLine *log_readLast();

LogLine *log_readNext(uint32_t id);
