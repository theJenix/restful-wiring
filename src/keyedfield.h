#ifndef __KEYED_FIELD_H
#define __KEYED_FIELD_H

#include "charbuffer.h"

class KeyedField {
private:
  const char *keyword;
  const char *ptr;
  bool matched;
  bool interpreted;
  int bytesToSkip;
public:
  KeyedField(const char *kw);
  virtual void read(const char *buf, int len);
  virtual bool interpret(const char *buf, int len);

  int getBytesToSkip();
  bool isInterpreted();
  bool isMatched();
};

class HeaderField : public KeyedField {
private:
  CharBuffer buffer;

public:
  HeaderField(const char *kw);

  const char *getValue();
  virtual bool interpret(const char *buf, int len);
};

class HTTPField : public KeyedField {
private:
  char codeBuf[4];
  int inx;
  int statusCode;

public:
  HTTPField();
  int getStatusCode();
  virtual bool interpret(const char *buf, int len);
};

#endif // __KEYED_FIELD_H
