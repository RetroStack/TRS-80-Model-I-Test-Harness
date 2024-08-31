#include "Set.h"

Set::Set() : size(0) {
  clear();
}

bool Set::add(const char* value) {
  if (!contains(value)) {
    if (size < MAX_SIZE) {
      strncpy(elements[size], value, MAX_LENGTH - 1);  // Copy the string safely
      elements[size][MAX_LENGTH - 1] = '\0';  // Ensure null termination
      size++;
      return true;
    }
    return false; // Set is full
  }
  return false; // Value already exists
}

void Set::print() const {
  // Serial.println("Set contents:");
  for (byte i = 0; i < size; i++) {
    Serial.print(elements[i]);
  }
}

void Set::clear() {
  for (byte i = 0; i < MAX_SIZE; i++) {
    elements[i][0] = '\0';  // Clear each string
  }
  size = 0;
}

bool Set::contains(const char* value) const {
  for (byte i = 0; i < size; i++) {
    if (strcmp(elements[i], value) == 0) return true;
  }
  return false;
}

byte Set::getSize() const {
  return size;
}