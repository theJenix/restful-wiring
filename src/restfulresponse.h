#ifndef __RESTFUL_RESPONSE_H
#define __RESTFUL_RESPONSE_H

#include "platform.h"
#include "keyedfield.h"
#include "Ethernet.h"

#define RESPONSE_READ_BUFFER_LEN 32

class RestfulResponse /* : public Schedulable TODO*/ {
private:
  Client& client;
  HTTPField statusCodeField;
  HeaderField contentLengthField;
  KeyedField bodyDelimField;
  CharBuffer body;
  int contentLength;
  bool inBody;
  bool ready;

  void markReadyAndDisconnect();

  int streamFromClient(char *buf, int maxLen);
public:
  RestfulResponse(Client& c);
  virtual ~RestfulResponse();

  // These 3 attributes are only valid if isReady == true
  const char *getBody();
  int getLength();
  int getStatusCode();
  bool isReady();
  bool run();
};

#endif // __RESTFUL_RESPONSE_H
