#ifndef __CHAR_BUFFER_H
#define __CHAR_BUFFER_H

class CharBuffer {
private:
  char *buffer;
  int sizeMult;
  int length;
  int capacity;
  int maxCapacity;

  void grow();
public:
  CharBuffer(int s);
  CharBuffer(int s, int m);
  virtual ~CharBuffer();

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
