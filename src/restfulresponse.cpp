
#include "restfulresponse.h"

static const bool debug = false;

/*private*/
void RestfulResponse::markReadyAndDisconnect() {
  if (debug) { Serial.println("markReadyAndDisconnect called"); }
  ready = true;
  client.flush();
  client.stop(); // Disconnect when we're done reading
}

/*private*/
int RestfulResponse::streamFromClient(char *buf, int maxLen) {
  int len = 0;
  for (uint8_t i = 0; i < maxLen; ++i) {
      if (!client.available()) {
          break;
      }
      char val = client.read();
      //TODO debug
      buf[i] = val;
      len++;
  }
//  Serial.print("Read: "); Serial.println(len);
  return len;
}

RestfulResponse::RestfulResponse(Client& c) : client(c), contentLengthField("Content-Length: "), bodyDelimField("\r\n\r\n"), body(RESPONSE_READ_BUFFER_LEN), contentLength(0), inBody(false), ready(false), timeoutTime(0), statusCode(STATUS_CODE_ERR_UNKNOWN) {}

RestfulResponse::~RestfulResponse() {
}

void RestfulResponse::cancel() {
  markReadyAndDisconnect();
}

const char *RestfulResponse::getBody() {
  return isReady() ? (const char *)body : NULL;
}

int RestfulResponse::getLength() {
  return isReady() ? body.getLength() : -1;
}

int RestfulResponse::getStatusCode() {
  return statusCode;//isReady() ? statusCodeField.getStatusCode() : -1;
}

bool RestfulResponse::isReady() {
  return ready;
}

bool RestfulResponse::run() {
//  Serial.println("Running");
  if (isReady()) {
    // Nothing to do...
    return true;
  }
  
  if (!client.connected()) {
    // Client disconnected...we'll take what we got
    markReadyAndDisconnect();
    return true;
  }

  if (timeoutTime > 0 && millis() >= timeoutTime) {
    markReadyAndDisconnect();
    statusCode = STATUS_CODE_ERR_TIMEOUT;
    return true;
  }

  char buf[RESPONSE_READ_BUFFER_LEN + 1] = {0};
  int len = 0;
  while ((len = streamFromClient(buf, RESPONSE_READ_BUFFER_LEN)) > 0) {
    buf[len] = 0;
    if (debug) { Serial.print("Received: "); Serial.print(len); Serial.print(", "); Serial.println(buf); }
    if (!inBody) {
      statusCodeField.read(buf, len);
      contentLengthField.read(buf, len);
      bodyDelimField.read(buf, len);
      if (statusCodeField.isInterpreted()) {
        statusCode = statusCodeField.getStatusCode();
      }
      inBody = bodyDelimField.isMatched();
      if (inBody) {
        if (debug) { Serial.println("In body!"); }
        if (contentLengthField.isInterpreted()) {
//          Serial.print("Content Length: "); Serial.println(contentLengthField.getValue());
          contentLength = atoi(contentLengthField.getValue());
          if (debug) { Serial.print("Content Length: "); Serial.println(contentLength); }
        } else {
          contentLength = 0;
        }
        int skip = bodyDelimField.getBytesToSkip();
        if (debug) { Serial.print("Skip: "); Serial.print(skip); Serial.print(", "); Serial.print(len); Serial.print(", "); Serial.println(len - skip); }
        // -1 because len is a length and skip is a 0-based index
        body.append(&buf[skip], len - skip);
      }
    } else {
  //    Serial.println("APPENDIN");
      body.append(buf, len);
    }
//    Serial.println("LEWPIN");
  }
//Serial.print("PRE_EXITIN:");
//Serial.println(inBody);
//  Serial.println(body.getLength());
//  Serial.println(body);
  // In some circumstances, the content length is smaller than the total bytes read...
  // Content-Length should be gospel, so we accept it and then truncate the body string
  // EXCEPT if the connection is closed.  We can also look for the Connection: close header
  // TODO: maybe have "expected lenght" as a field in CharBuffer, so this becomes if (inBody && body.receivedAll())...
  if (inBody && ((contentLength > 0 && contentLength <= body.getLength()) ||
                 (contentLength == 0 && !client.connected()))) {
    if (contentLength > 0) {
      body.truncAt(contentLength);
    } else { //TODO: this should be automatic
      body.truncAt(body.getLength());
    }
    markReadyAndDisconnect();
  }

  if (debug) { Serial.print("Exiting.  Ready? "); Serial.println(ready ? "true" : "false"); }
  return ready;
}
#if 0

    int pos = strstr(buffer, bodyDelim);
    if (pos >= 0) {
      pos += strlen(bodyDelim);
    } else {
      char buf[5] = {0};
      buffer.getAcross(2, 2, buf);
      if (strstr(buf, "\r\n\r\n")) {
      }
    }
    // Swap the active buffer
    buf = buffer.swap();
    toRead = buffer.length;
#endif
#if 0
    memcpy(&response[responseLength], buf, len);
    resopnseLength += len;
#endif

void RestfulResponse::setTimeoutTime(long timeoutTime) {
  this->timeoutTime = timeoutTime;
}
