#include <iostream>

#include "ofxhome.h"

using ofxget::Institution;
using ofxget::OfxHomeFullDumpString;
using ofxget::OfxDumpStringToInstitutions;
using std::cout;
using std::endl;

int main() {
  try {
    string dump = OfxHomeFullDumpString();
    cout << dump << endl;
    vector<Institution> insts = OfxDumpStringToInstitutions(dump);
    //for (const auto& i : insts) {
    //  cout << i << endl;
    //}
  } catch(const string& msg) {
    cout << msg << endl;
  }

  return 0;
}
