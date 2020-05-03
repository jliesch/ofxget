#ifndef OFXHOME_H
#define OFXHOME_H

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;

namespace ofxget {

struct Institution {
    int ofxhome_id;
    string name;
    string fid;
    string org;
    string url;
    int ofxfail;
    int sslfail;
    string lastofxvalidation;
    string lastsslvalidation;
    string brokerid;
    string bankid;
    map<string, string> profile;

    friend std::ostream& operator<<(std::ostream& stream,
                                    const Institution& inst);
};

string OfxHomeFullDumpString();

vector<Institution> OfxDumpStringToInstitutions(const string& dump);

const Institution* FindInstitutionByName(
    const vector<Institution>& institutions,
    const string& name);


// Upload a successful request to OFX Home. The request string will be sanitized
// server side, however, to be safe AnonymizeRequest is called beforehand.
string UploadSuccessfulRequest(const string& api_key, const string& url, const string& request);

// Returns true if the api_key is avalid.
bool ValidateApiKey(const std::string& api_key);

// Remove any personal information from USERID, ACCTID, and USERPASS fields.
string AnonymizeRequest(const string& request);

}  // namespace ofxget

#endif // OFXHOME_H
