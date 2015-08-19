
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
  client.print(string); //, strlen(string));
  delay(10);
}

/*protected*/
RestfulOperation& RestfulOperation::methodAndUri(const char *method, const char *uriPre, const char *uri) {
  write(method);
  write(" ");
  if (uriPre && *uriPre) {
    write(uriPre);
  }
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
    if (debug) { Serial.println("Deleting the response"); }
    delete theResponse;
  }
  if (debug) { Serial.println("Allocating the response"); }
  theResponse = new RestfulResponse(client);
}
/*private*/
bool RestfulClient::connect() {
  uint8_t addr[4] = {0};
//  int count = sscanf(host, "%hhu%.%hhu%.%hhu%.%hhu", &addr[0], &addr[1], &addr[2], &addr[3]);
  int count = parseIP(host, addr);
#if 0
  if (count < 4) {
    count = 4;

    static uint32_t ip_addr = 0;


    if (ip_addr == 0)
      gethostbyname((char*)host, strlen(host), &ip_addr);
    addr[0] = BYTE_N(ip_addr, 3);
    addr[1] = BYTE_N(ip_addr, 2);
    addr[2] = BYTE_N(ip_addr, 1);
    addr[3] = BYTE_N(ip_addr, 0);
  }
#endif
  if (count < 4) {
    Serial.print("Connecting to host: "); Serial.println(host);
    bool conn = client.connect(host, port);
    Serial.print("Connected? ");  Serial.println(conn ? "true" : "false");
    // Delay seems to be required for interfacing Spark Core with a heroku app (otherwise, it
    // can get into a mode where it never resolves the host name).
    delay(100);
    return conn;
  } else {
    Serial.print("Connecting to address: "); Serial.print(addr[0]); Serial.print("."); Serial.print(addr[1]);Serial.print("."); Serial.print(addr[2]); Serial.print("."); Serial.println(addr[3]);
    bool conn = client.connect(addr, port);
    Serial.print("Connected? ");  Serial.println(conn ? "true" : "false");
    delay(100);
    return conn;
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
  op.methodAndUri(method, pathPrefix, path);
}

/*private*/
void RestfulClient::writeStandardHeaders(RestfulOperation& op) {
  op.header("Content-Type", "application/x-www-form-urlencoded");
  op.header("Connection", "close");
  op.header("Host", host);
}

RestfulClient::RestfulClient(const char *host, int port, const char *pathPrefix) : theResponse(NULL) {
  this->host = (char *)malloc(strlen(host) + 1);
  strcpy(this->host, host);
  this->port = port;
  this->pathPrefix = (char *)malloc(strlen(pathPrefix) + 2);
  char *ptr = this->pathPrefix;
  if (*pathPrefix != 0 && *pathPrefix != '/') {
    *ptr++ = '/';
  }
  strcpy(ptr, pathPrefix);
}

RestfulClient::~RestfulClient() {
  if (theResponse) {
    delete theResponse;
  }
  free(this->host);
  free(this->pathPrefix);
}

RestfulOperation RestfulClient::get(const char *uri) {
  bool connected = connect();
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "GET", uri);
    writeStandardHeaders(op);
  }
  return op;
}

RestfulOperation RestfulClient::post(const char *uri) {
  bool connected = connect();
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "POST", uri);
    writeStandardHeaders(op);
  }
  return op;
}

RestfulOperation RestfulClient::put(const char *uri) {
  bool connected = connect();
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "PUT", uri);
    writeStandardHeaders(op);
  }
  return op;
}

RestfulOperation RestfulClient::del(const char *uri) {
  bool connected = connect();
  clearTheResponse();
  RestfulOperation op(client, *theResponse, host);
  if (connected) {
    Serial.println("Connected");
    writeMethodAndPath(op, "DELETE", uri);
    writeStandardHeaders(op);
  }
  return op;
}

