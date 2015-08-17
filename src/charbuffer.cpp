#include "charbuffer.h"
#include "platform.h"

/*private*/
void CharBuffer::grow() {
  if (capacity == 0) {
    capacity = sizeMult;
  } else {
    capacity *= 2;
  }
  buffer = (char *)realloc(buffer, capacity * sizeof(char));
}

CharBuffer::CharBuffer(int s) : sizeMult(s), buffer(NULL), length(0), capacity(0) {}

void CharBuffer::append(const char *buf, int bufLen) {
  if (length + bufLen > capacity) {
    grow();
  }

  memcpy(&buffer[length], buf, bufLen);
  length += bufLen;
}

void CharBuffer::truncAt(int pos) {
  //FIXME: we assume pos < length and pos >= 0
  buffer[pos] = 0;
  length = pos + 1;
}

