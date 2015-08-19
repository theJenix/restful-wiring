#include "charbuffer.h"
#include "platform.h"

static const bool debug = false;

/*private*/
void CharBuffer::grow() {
  int newCap = capacity;
  if (newCap == 0) {
    newCap = sizeMult;
  } else {
    newCap *= 2;
  }
  if (maxCapacity > 0) {
    newCap = min(newCap, maxCapacity);
  }
  if (newCap == capacity) {
    if (debug) { Serial.print("Already at max capacity "); Serial.println(capacity); }
  } else {
    capacity = newCap;
    if (debug) { Serial.print("Expanding capacity to: "); Serial.println(capacity); }
    buffer = (char *)realloc(buffer, capacity * sizeof(char));
  }
}

CharBuffer::CharBuffer(int s) : sizeMult(s), buffer(NULL), length(0), capacity(0), maxCapacity(0) {}
CharBuffer::CharBuffer(int s, int m/*, bool f*/) : sizeMult(s), maxCapacity(m), /*firstIn(f),*/ buffer(NULL), length(0), capacity(0) {}

CharBuffer::~CharBuffer() {
  free(buffer);
}

void CharBuffer::append(const char *buf, int bufLen) {
  int toCopy = bufLen;
  if (maxCapacity > 0) {
    //TODO: implement first in or last in strategies...for now, first in is ok
    toCopy = min(bufLen, maxCapacity - length);
  }
  while (length + toCopy > capacity) {
    grow();
  }

  if (debug) { Serial.print("Copying "); Serial.print(toCopy); Serial.print(" of "); Serial.print(bufLen); Serial.print(" chars into buffer with "); Serial.print(capacity - length); Serial.println(" capacity"); }
  memcpy(&buffer[length], buf, toCopy);
  length += toCopy;
}

void CharBuffer::truncAt(int pos) {
  if (debug) { Serial.print("CharBuffer::truncAt "); Serial.println(pos); }
  //FIXME: we assume pos < length and pos >= 0
  buffer[pos] = 0;
  length = pos;
  if (debug) { Serial.print("CharBuffer contains "); Serial.print(length); Serial.print(", "); Serial.println(buffer); }
}

