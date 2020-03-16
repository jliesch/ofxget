#include <iostream>

#include "ofxhome.h"

void assertEq(const string& actual, const string& expected) {
  if (actual != expected) {
    std::cout << '"' << actual << '"'  << " != " << '"' << expected << '"'
              << std::endl;
  }
}

int main() {
  assertEq(AnonymizeRequest("<USERID>123"), "<USERID>X\n");
  assertEq(AnonymizeRequest("sdf\n<USERID>123<OTHER>"), "sdf\n<USERID>X\n<OTHER>");
  assertEq(AnonymizeRequest("<USERID>123<USERID>456"), "<USERID>X\n<USERID>X\n");
  assertEq(AnonymizeRequest("<ACCTID>123"), "<ACCTID>X\n");
  assertEq(AnonymizeRequest("<USERPASS>123"), "<USERPASS>X\n");
  return 0;
}
