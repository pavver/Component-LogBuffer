#include "include/LogBuffer.h"
#include <cstring>
#include <stdarg.h>
#include <stdio.h>

static FlexibleCircularBuffer<char> logBuffer = FlexibleCircularBuffer<char>();

int log_toBuffer(const char *fmt, va_list list)
{
  size_t len = vsnprintf(nullptr, 0, fmt, list) + 1;
  char *line = (char *)malloc(len);
  if (!line)
    return 0;

  vsnprintf(line, len, fmt, list);

  logBuffer.WriteLine(line, len);

  free(line);

  return vprintf(fmt, list);
}

LogLine *log_readFirst()
{
  return (LogLine *)logBuffer.ReadFirst();
}

LogLine *log_readLast()
{
  return (LogLine *)logBuffer.ReadLast();
}

LogLine *log_readNext(uint32_t id)
{
  return (LogLine *)logBuffer.ReadNext(id);
}
