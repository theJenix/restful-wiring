/**
 *
 */

#ifndef __RESTFUL_H
#define __RESTFUL_H
#if 0
class RestfulTransport {
private:
  const IPAddress& localIP;
  Client& client;
public:
  RestfulTransport(Client& c, const IPAddress& l);
};
#endif

#include "Ethernet.h"
#include "restfulresponse.h"

class RestfulOperation {
private:
  Client& client;
  RestfulResponse& theResponse;

  //TODO: roll this into client, or into RestfulTransport
  const char *host; //const IPAddress& localIP;
  bool dirty;

  void flushIfDirty();
  void write(const char *string);

protected:
  RestfulOperation(Client& c, RestfulResponse& theResponse, const char *h); //const IPAddress& localIP);
  
  RestfulOperation& methodAndUri(const char *method, const char *uri);

public:
  RestfulOperation& header(const char *key, const char *value);
  RestfulOperation& body(const char *body);

//  void doRequest();

  /**
   * Get a response object that will capture the result of the operation.
   * This is an asynchronous response, and requires the caller to call "run"
   * frequently (possibly in a main loop or from a job scheduler).
   */
  RestfulResponse& response();

  /**
   * Get a response object that will capture the result of the operation.
   *
   * This function blocks and waits for the response to come in before returning.
   */
  RestfulResponse& waitForResponse();
  
  friend class RestfulClient;
};

class RestfulClient {
private:
  TCPClient client;
  //FIXME: this is a punt...we need to implement = operators and copy constructors, but until then
  // just assume one response at a time
  RestfulResponse *theResponse;
//  const IPAddress& localIP;
  const char *host;
  int port;

  void clearTheResponse();
  bool connect();
  int  parseIP(const char *possibleIP, uint8_t addr[4]);
  void writeMethodAndPath(RestfulOperation& op, const char *method, const char *uri);
  void writeStandardHeaders(RestfulOperation& op);

public:
  RestfulClient(const char *host, int port); //const IPAddress& localIP);

  /**
   * Request a resource from the connected system.
   */ 
  RestfulOperation get(const char *uri); 

  /**
   * Post a new resource to the connected system.
   */
  RestfulOperation post(const char *uri); 

  /**
   * Update an existing resource in the connected system
   */
  RestfulOperation put(const char *uri); 

  /**
   * Delete a resource on the connected system.
   */
  RestfulOperation del(const char *uri); 
};

#endif //__RESTFUL_H
