
//#define FIELD_DEBUG
#include "platform.h"

#include "keyedfield.h"
#include <stdlib.h>     /* atoi */
#include <string.h>

static const bool debug = false;

void printChar(char c) {
  if (c == '\r') {
    Serial.print("<CR>");
  } else if (c == '\n') {
    Serial.print("<LF>");
  } else {
    Serial.print(c);
  }
}

KeyedField::KeyedField(const char *kw) : keyword(kw), ptr(keyword), matched(false), interpreted(false), bytesToSkip(0) {}

//TODO: this whole file needs another pass when I'm less tired...
void KeyedField::read(const char *buf, int len) {
  // Nothing to do
  if (interpreted) {
    return;
  }
  if (debug) { Serial.print("("); Serial.print(keyword); Serial.print(") Reading: "); Serial.print(len); Serial.print(", "); Serial.println(buf); }
  // Try to start a match
  int start = 0;
  if (!matched) {
    if (ptr == keyword) {
      if (debug) { Serial.println("Not in a match yet."); }
      for (int ii = 0; ii < len; ++ii) {
        if (debug) { printChar(*ptr); Serial.print("=?"); printChar(buf[ii]); Serial.print(", "); }
        if (*ptr == buf[ii]) {
          if (debug) { Serial.println("Found a match!"); }
          // Found a match!
          ptr++;
          start = ii + 1;
          break;
        }
      }
    } 
    
    // In the middle of a match
    if (ptr != keyword) {
      if (debug) { Serial.print("In a match.  Left: "); Serial.println(ptr); }
      for (int ii = start; ii < len && !matched; ++ii) {
        if (debug) { printChar(*ptr); Serial.print("=?"); printChar(buf[ii]); Serial.print(", "); }
        if (*ptr == buf[ii]) {
          ptr++;
          start++;
        } else {
          // Reset the match
          ptr = keyword;
          break;
        }
        // End of the keyword, we've found a match
        matched = *ptr == 0;
      }
      if (debug) { Serial.println(); }
    }
  }

  bytesToSkip = start;
  if (matched) {
    if (debug) { Serial.print("Found a match: "); Serial.print(keyword); Serial.print(", To be interpreted: "); Serial.println(&buf[start]); }
    interpreted = interpret(&buf[start], len - start);
    if (debug) {
      if (interpreted) {
        Serial.println("Interpreted!");
      } else {
        Serial.println("More to interpret...");
      }
    }
  }
  if (debug) { Serial.println("Exiting KeyedField::read"); }
}

int KeyedField::getBytesToSkip() {
  return bytesToSkip;
}

bool KeyedField::interpret(const char *buf, int len) {
  return true; // Nothing to interpret, beyond the match.
}

bool KeyedField::isInterpreted() {
  return interpreted;
}

bool KeyedField::isMatched() {
  return matched;
}

HeaderField::HeaderField(const char *kw) : KeyedField(kw), buffer(8) {}

const char *HeaderField::getValue() {
  return buffer;
}

bool HeaderField::interpret(const char *buf, int len) {
  buffer.append(buf, len);
  char *crlf = strstr(buffer, "\r\n");
  if (crlf) {
    buffer.truncAt(crlf - buffer);
    return true;
  } else {
    return false;
  }
}

HTTPField::HTTPField() : KeyedField("HTTP/1.1"), statusCode(-1), inx(0) {
  memset(codeBuf, 0, 4); //codeBuf = {0};
} 

int HTTPField::getStatusCode() {
  return statusCode;
}

bool HTTPField::interpret(const char *buf, int len) {
  for (int ii = 0; ii < len && inx < 3; ++ii) {
    switch(buf[ii]) {
      case '0': case '1': case '2': case '3': case '4':
      case '5': case '6': case '7': case '8': case '9':
        codeBuf[inx++] = buf[ii];
    }
  }

  if (inx == 3) {
    codeBuf[inx] = 0;
    statusCode = atoi(buf);
    return true;
  } else {
    return false;
  }
}
