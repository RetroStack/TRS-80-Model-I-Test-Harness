#ifndef SET_H
#define SET_H

#include <Arduino.h>

#define MAX_SIZE 16
#define MAX_LENGTH 4  // Maximum length of strings including null terminator

class Set {
public:
  Set();
  bool add(const char* value);
  void print() const;
  void clear();
  byte getSize() const;  // Get the number of elements in the set  

private:
  char elements[MAX_SIZE][MAX_LENGTH];  // 2D array for strings
  byte size;
  bool contains(const char* value) const;
};

#endif // SET_H
