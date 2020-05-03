#ifndef __OFX_GET_H__
#define __OFX_GET_H__

#include <map>
#include <string>

#include "ofxhome.h"

namespace ofxget {

// ofxget is a small library for building OFX requests and downloading OFX
// responses from institutions. It is intentionally lightweight and simple.
//
// A simple use of the library is as follows:
//
//   try {
//     VarsMap vars;
//     InitVars(&vars);
//     AddApp(&vars, "QuickBooks_2008");
//     AddInstitution(479, &vars);  // 479 is Vanguard
//     AddPasswordsForTest(479, "passwords.txt", &vars);
//     // Note: AddPasswordsForTest should only be used for testing. Othwerise,
//     // set vars["USERID"] = "user", etc.
//     
//     string request = BuildRequest(
//       GetRequestTemplate("requests/" + string(request_filename)), vars);
//     string response = PostRequest(request, vars["URL"]);
//     cout << "RESPONSE" << endl << endl << response << endl;
//   } catch(const string& msg) {
//     cout << msg << endl;
//   }

// Request timeout when posting requests in seconds.
#define REQUEST_TIMEOUT 10

// VarsMap is one half of the data used for building a request. The other is the
// request template. VarsMap contains all of the variables that will be
// substituted into the request. For example, VarMap["USERID"] = "myid".
typedef map<string, string> VarsMap;

class OfxGetContext {
 public:
  OfxGetContext();

  // Return to its original state.
  void Reset();

  // Add application vars. Using "Quicken_2011" is a safe bet. See ofxget_apps.h
  // for a comprehensive list of app names.
  OfxGetContext& AddApp(const string& name);

  // Add institution vars. The id value refers to an institution in the
  // institutions.txt file.
  OfxGetContext& AddInstitution(int id);

  // Add user passwords and other account info to a VarsMap. These values are
  // read from a plain text file. This is not safe and should only be used for
  // testing purposes.
  OfxGetContext& AddPasswordsForTest(int id, const char* filename);

  // Read a request template from a file (eg, investment.txt). Returns empty
  // string on error.
  string GetRequestTemplate(const string& filename);

  // Substitute the vars into a request template. A fully working request is
  // returned.
  OfxGetContext& AddRequestTemplate(const string& request_template);

  // Send a request to an OFX server. url will be taken from a VarsMap using the
  // URL key if the institution was loaded from AddInstitution.
  OfxGetContext& PostRequest();

  // Return the request based on the request template and vars. On error, an
  // empty string is returned.
  string request();

  // Return the response. Only populated after calling PostRequest. On error,
  // an empty string is returned.
  const string& response() { return response_; }

  bool is_error() { return !error_string_.empty(); }
  const string& error_string() { return error_string_; }

 public: // TODO: private
  VarsMap vars_map_;
  string error_string_;
  string request_template_;
  string response_;
};

// Initialize a vars map with common variables needed to send an OFX request.
void InitVars(VarsMap* vars);

// Return a vector of all the request vars that are missing.
vector<string> GetMissingRequestVars(const string& request_template,
                                     const VarsMap& vars);


} // namespace: ofxget

#endif /* __OFX_GET_H__ */
