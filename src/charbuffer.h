#ifndef __CHAR_BUFFER_H
#define __CHAR_BUFFER_H

class CharBuffer {
private:
  char *buffer;
  int sizeMult;
  int length;
  int capacity;

  void grow();
public:
  CharBuffer(int s);

  operator const char *() const {
    return buffer;
  }

  int getLength() {
    return length;
  }

  void append(const char *buf, int bufLen);

  void truncAt(int pos);
};
#endif //__CHAR_BUFFER_H
