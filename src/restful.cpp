
#include "restful.h"

static const bool debug = false;

//TODO: for now, this just writes as header and body are called.  this is easier to code for now, but limits flexibility (header MUST be called before body or else bad things happen).  Good enough for now...
RestfulOperation::RestfulOperation(Client& c, RestfulResponse& r, const char *h) : client(c), theResponse(r), host(h), dirty(false) {}

/*private*/
void RestfulOperation::flushIfDirty() {
  if (dirty) {
    client.flush();
    dirty = false;
  }
}

/*private*/
void RestfulOperation::write(const char *string) {
  dirty = true;
  if (debug) { Serial.print("Writing: "); Serial.println(string); }
  //TODO: debug info
  client.write((uint8_t *)string, strlen(string));
}

/*protected*/
RestfulOperation& RestfulOperation::methodAndUri(const char *method, const char *uri) {
  write(method);
  write(" ");
  write(uri);
  write(" HTTP/1.1\r\n"); 
  return *this;
}

RestfulOperation& RestfulOperation::header(const char *key, const char *value) {
  write(key);
  write(": ");
  write(value);
  write("\r\n");
  return *this; 
}

RestfulOperation& RestfulOperation::body(const char *body) {
  int contentLen = strlen(body);
  char buf[16];
  sprintf(buf, "%d", contentLen);
  header("Content-Length", buf);
  write("\r\n");
  write("\r\n");
  write(body);
  write("\r\n");
  return *this; 
} 

RestfulResponse& RestfulOperation::response() {
  // This crlf finishes the request
  write("\r\n");
  flushIfDirty();
  return theResponse; //RestfulResponse(client);
}

RestfulResponse& RestfulOperation::waitForResponse() {
  // This crlf finishes the request
  write("\r\n");
  flushIfDirty();
  while (!theResponse.isReady()) {
    theResponse.run();
    delay(10);
  }

  return theResponse;
}

/*private*/
void RestfulClient::clearTheResponse() {
  if (theResponse) {
    delete theResponse;
  }
  theResponse = new RestfulResponse(client);
}
/*private*/
bool RestfulClient::connect() {
  uint8_t addr[4] = {0};
//  int count = sscanf(host, "%hhu%.%hhu%.%hhu%.%hhu", &addr[0], &addr[1], &addr[2], &addr[3]);
  int count = parseIP(host, addr);
  if (count < 4) {
    Serial.print("Connecting to host: "); Serial.println(host);
    return client.connect(host, port);
  } else {
    Serial.print("Connecting to address: "); Serial.println(host);
    return client.connect(addr, port);
  }
}

/*private*/
int RestfulClient::parseIP(const char *possibleIP, uint8_t addr[4]) {
//  int run = 0;
  char buf[16] = {0};
  strncpy(buf, possibleIP, min(15, strlen(possibleIP)));
  char *part = strtok(buf, ".");
#if 0
  bool num = true;
  for (int ii = 0; ii < strlen(part); ii++) {
    if (!isdigit(part[ii])) {
      num = false;
    }
  while (part && ) {
#endif
  int partCount = 0;
  while (part && partCount < 4) {
    addr[partCount++] = atoi(part);
    part = strtok(NULL, ".");
  }

  return partCount;
}

/*private*/
void RestfulClient::writeMethodAndPath(RestfulOperation& op, const char *method, const char *path) {
  op.methodAndUri(method, path);
}

/*private*/
void RestfulClient::writeStandardHeaders(RestfulOperation& op) {
  op.header("Content-Type", "application/x-www-form-urlencoded");
  op.header("Connection", "close");
  op.header("Host", host);
}

RestfulClient::RestfulClient(const char *host, int port) : theResponse(NULL), host(host), port(port) {}

RestfulOperation RestfulClient::get(const char *uri) {
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  bool connected = connect();
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "GET", uri);
    writeStandardHeaders(op);
  }
  return op;
}

RestfulOperation RestfulClient::post(const char *uri) {
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  bool connected = connect();
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "POST", uri);
    writeStandardHeaders(op);
  }
  return op;
}

RestfulOperation RestfulClient::put(const char *uri) {
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  bool connected = connect();
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "PUT", uri);
    writeStandardHeaders(op);
  }
  return op;
}

RestfulOperation RestfulClient::del(const char *uri) {
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  bool connected = connect();
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "DELETE", uri);
    writeStandardHeaders(op);
  }
  return op;
}

