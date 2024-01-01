#pragma once

#include <stdarg.h>
#include <stdlib.h>

// Розмір текстового циклічного буферу лога
#define LOG_BUFF_LENGTH 4096

// Максимальна кількість окремих стрічок логу
#define LOG_MAX_LINES 128

struct LogLine
{
public:
  const char *GetLogString() const
  {
    return _line;
  }

  uint16_t GetId() const
  {
    return _id;
  }

  ~LogLine()
  {
    if (_needFree)
      free(_line);
  }

protected:
  LogLine(char *line, uint16_t id, bool needFree)
      : _line(line),
        _id(id),
        _needFree(needFree)
  {
  }

  char *_line;
  const uint16_t _id;
  const bool _needFree;
};

int log_toBuffer(const char *fmt, va_list list);

LogLine *log_readFirst();

LogLine *log_readLast();

LogLine *log_readNext(uint16_t id);