#ifndef __RESTFUL_RESPONSE_H
#define __RESTFUL_RESPONSE_H

#include "platform.h"
#include "keyedfield.h"
#include "Ethernet.h"

#define RESPONSE_READ_BUFFER_LEN 32

#define STATUS_CODE_ERR_UNKNOWN -1
#define STATUS_CODE_ERR_TIMEOUT -2

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
  long timeoutTime;
  int statusCode;

  void markReadyAndDisconnect();

  int streamFromClient(char *buf, int maxLen);
public:
  RestfulResponse(Client& c);
  virtual ~RestfulResponse();

  void cancel();
  // These 3 attributes are only valid if isReady == true
  const char *getBody();
  int getLength();
  int getStatusCode();
  bool isReady();
  bool run();
  void setTimeoutTime(long timeoutTime);
};

#endif // __RESTFUL_RESPONSE_H
