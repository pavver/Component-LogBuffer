#include "include/LogBuffer.h"
#include <cstring>
#include <stdarg.h>
#include <stdio.h>

class EditableLogLine : public LogLine
{
public:
  EditableLogLine(char *line, uint16_t index, bool needFree = false)
      : LogLine(line, index, needFree)
  {
  }
};

struct BuffLogLine
{
public:
  int16_t startIndex = 0;
  int16_t endIndex = 0;
  uint16_t id = 0;

  bool IsIntersection(BuffLogLine &line)
  {
    if (startIndex < endIndex && line.startIndex < line.endIndex)
    {
      if (startIndex <= line.startIndex)
        return endIndex >= line.startIndex;
      return line.endIndex >= startIndex;
    }
    if (startIndex > endIndex && line.startIndex > line.endIndex)
      return true;
    if (startIndex > endIndex)
    {
      return line.startIndex <= endIndex || line.endIndex >= startIndex;
    }
    return startIndex <= line.endIndex || endIndex >= line.startIndex;
  }
};

struct LogCircularBuffer
{
public:
  LogCircularBuffer()
  {
  }

  uint16_t writeString(char *str, uint16_t length)
  {
    if (length == 0)
      return 0;

    int16_t nextIndex = getNextIndex(_indexLastLine);

    BuffLogLine newLine;
    newLine.endIndex = 0;

    if ((_line[_indexLastLine].endIndex + length + 1) < LOG_BUFF_LENGTH)
    {
      memcpy(_indexLastLine == -1 ? buff : (buff + _line[_indexLastLine].endIndex + 1), str, length + 1);
      newLine.endIndex = (_indexLastLine == -1 ? 0 : _line[_indexLastLine].endIndex + 1) + length;
    }
    else
    {
      memcpy(_indexLastLine == -1 ? buff : (buff + _line[_indexLastLine].endIndex + 1), str, LOG_BUFF_LENGTH - _line[_indexLastLine].endIndex - 1);
      newLine.endIndex = LOG_BUFF_LENGTH - _line[_indexLastLine].endIndex - 1;
      str += newLine.endIndex;
      length -= newLine.endIndex;

      memcpy(buff, str, length + 1);
      newLine.endIndex = length;
    }

    if (_indexLastLine == -1)
    {
      newLine.startIndex = 0;
      newLine.id = 0;
    }
    else
    {
      newLine.startIndex = (_line[_indexLastLine].endIndex + 1) % LOG_BUFF_LENGTH;
      newLine.id = _line[_indexLastLine].id + 1;
    }

    if (_indexFirstLine == -1)
      _indexFirstLine = nextIndex;
    else
    {
      BuffLogLine line = _line[_indexFirstLine];
      bool isIntersection = line.IsIntersection(newLine);
      while (isIntersection || _indexFirstLine == nextIndex)
      {
        _indexFirstLine = getNextIndex(_indexFirstLine);
        line = _line[_indexFirstLine];
        isIntersection = line.IsIntersection(newLine);
      }
    }

    _indexLastLine = nextIndex;
    _line[_indexLastLine] = newLine;

    return newLine.id;
  }

  LogLine *readFirst()
  {
    if (_indexFirstLine < 0)
      return nullptr;
    return CreateLine(_indexFirstLine);
  }

  LogLine *readLast()
  {
    if (_indexLastLine < 0)
      return nullptr;
    return CreateLine(_indexLastLine);
  }

  LogLine *readNext(uint16_t id)
  {
    if (_indexLastLine < 0)
      return nullptr;

    int16_t index = _indexFirstLine;

    while (true)
    {
      if (index == _indexLastLine)
        return nullptr;

      if (_line[index].id == id)
        return CreateLine(getNextIndex(index));

      index = getNextIndex(index);
    }
  }

private:
  char buff[LOG_BUFF_LENGTH];
  BuffLogLine _line[LOG_MAX_LINES];

  // індекс стрічки яка була записана самою першою
  int16_t _indexFirstLine = -1;

  // Індекс стрічки яка була записана самою останньою
  int16_t _indexLastLine = -1;

  int16_t getNextIndex(int16_t index)
  {
    return (index + 1) % LOG_MAX_LINES;
  }

  int16_t getPrevIndex(int16_t index)
  {
    return (index + LOG_MAX_LINES - 1) % LOG_MAX_LINES;
  }

  LogLine *CreateLine(int16_t index)
  {
    if (_line[index].startIndex < _line[index].endIndex)
      return new EditableLogLine(&buff[_line[index].startIndex], _line[index].id, false);

    uint16_t length = LOG_BUFF_LENGTH - _line[index].startIndex + _line[index].endIndex + 1;
    char *str = (char *)malloc(sizeof(char) * length);

    memcpy(str, buff + _line[index].startIndex, LOG_BUFF_LENGTH - _line[index].startIndex);
    memcpy(str + LOG_BUFF_LENGTH - _line[index].startIndex, buff, _line[index].endIndex);
    str[length - 1] = 0;
    return new EditableLogLine(str, _line[index].id, true);
  }
};

static LogCircularBuffer logBuffer = LogCircularBuffer();

int log_toBuffer(const char *fmt, va_list list)
{
  size_t len = vsnprintf(nullptr, 0, fmt, list) + 1;
  char *line = (char *)malloc(len);
  if (!line)
    return 0;

  vsnprintf(line, len, fmt, list);

  logBuffer.writeString(line, len);

  free(line);

  return vprintf(fmt, list);
}

LogLine *log_readFirst()
{
  return logBuffer.readFirst();
}

LogLine *log_readLast()
{
  return logBuffer.readLast();
}

LogLine *log_readNext(uint16_t id)
{
  return logBuffer.readNext(id);
}