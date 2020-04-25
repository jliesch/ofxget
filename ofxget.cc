#include <cstring>
#include <iostream>
#include <map>
#include <fstream>
#include <string>
#include <vector>

#include <curl/curl.h>

#include "pugixml/pugixml.hpp"

#include "ofxget.h"
#include "ofxget_apps.h"

namespace ofxget {

using std::cout;
using std::endl;
using std::map;
using std::ostream;
using std::string;
using std::vector;

// Forward declarations
static size_t CurlWriteToString(char *ptr, size_t size, size_t nmemb, void *userdata);
size_t HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata);

OfxGetContext::OfxGetContext() {
  InitVars(&vars_map_);
}

OfxGetContext& OfxGetContext::AddApp(const string& name) {
  if (is_error()) return *this;
  auto apps = OfxApps();
  for (std::size_t i = 0; i < apps.size(); i++) {
    if (apps[i].name == name) {
      vars_map_["APPID"] = apps[i].appid;
      vars_map_["APPVER"] = apps[i].appver;
      return *this;
    }
  }
  error_string_ = "Unknown app " + name;
  return *this;
}

OfxGetContext& OfxGetContext::AddInstitution(int id) {
  if (is_error()) return *this;
  std::string id_str = std::to_string(id);
  string filename = "institutions.txt";
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename.c_str());
  if (!result) {
    error_string_ = "Could not parse " + filename;
    return *this;
  }
  auto inst = doc.find_child_by_attribute("institution", "id", id_str.c_str());
  if (inst.empty()) {
    error_string_ = "Could not find institution " + id_str;
    return *this;
  }
  std::vector<string> var_names {"org", "fid", "brokerid", "bankid", "url"};
  for (string name : var_names) {
    auto var = inst.child(name.c_str());
    if (var) {
      for (std::size_t i = 0; i < name.size(); i++) {
        name[i] = toupper(name[i]);
      }
      vars_map_[name] = var.child_value();
    }
  }
  return *this;
}

OfxGetContext& OfxGetContext::AddPasswordsForTest(
    int id, const char* filename) {
  if (is_error()) return *this;
  std::string id_str = std::to_string(id);
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_file(filename);
  if (!result) {
    return *this;
  }
  auto inst = doc.find_child_by_attribute("institution", "id", id_str.c_str());
  if (inst.empty()) {
    return *this;
  }
  for (auto node : inst) {
    vars_map_[node.name()] = node.child_value();
  }
  return *this;
}

string OfxGetContext::GetRequestTemplate(const string& filename) {
  if (is_error()) return "";
  std::ifstream f(filename);
  if (!f.is_open()) {
    error_string_ = "Could not open " + filename;
    return "";
  }

  string line;
  string request_template;
  while (std::getline(f, line)) {
    request_template += line + "\r\n";
  }

  return request_template;
}

OfxGetContext& OfxGetContext::AddRequestTemplate(
    const string& request_template) {
  if (is_error()) return *this;
  request_template_ = request_template;
  return *this;
}

string OfxGetContext::request() {
  if (is_error()) return "";
  string subbed = request_template_;
  for (std::size_t i = 0; i < subbed.size(); i++) {
    if (subbed[i] == '$') {
      std::size_t j;
      for (j = i + 1; j < subbed.size(); j++) {
        if (!isupper(subbed[j])) {
          break;
        }
      }
      string var = subbed.substr(i + 1, j - i - 1);
      VarsMap::const_iterator it = vars_map_.find(var);
      if (it != vars_map_.end()) {
        subbed.replace(i, j - i, it->second);
      } else {
        error_string_ = "Unspecified variable: " + var;
        return "";
      }
    }
  }
  return subbed;
}

OfxGetContext& OfxGetContext::PostRequest() {
  response_.clear();
  if (is_error()) return *this;
    // Fake account
    if (0) { response_ = R"FAKE(OFXHEADER:100
                  DATA:OFXSGML
                  VERSION:102
                  SECURITY:NONE
                  ENCODING:USASCII
                  CHARSET:1252
                  COMPRESSION:NONE
                  OLDFILEUID:NONE
                  NEWFILEUID:20180421105945.000

                  <OFX>
                  <SIGNONMSGSRSV1>
                  <SONRS>
                  <STATUS>
                  <CODE>0
                  <SEVERITY>INFO
                  <MESSAGE>Successful Sign On
                  </STATUS>
                  <DTSERVER>20180421125958[-5:EST]
                  <LANGUAGE>ENG
                  <DTPROFUP>20140605083000
                  <FI>
                  <ORG>Vanguard
                  <FID>15103
                  </FI>
                  <SESSCOOKIE>xxx
                  </SONRS>
                  </SIGNONMSGSRSV1>
                  <SIGNUPMSGSRSV1>
                  <ACCTINFOTRNRS>
                  <TRNUID>20180421105945.000
                  <STATUS>
                  <CODE>0
                  <SEVERITY>INFO
                  <MESSAGE>AcctInfoTrnRsV1 is successful
                  </STATUS>
                  <CLTCOOKIE>1
                  <ACCTINFORS>
                  <DTACCTUP>20180421125958[-5:EST]
                  <ACCTINFO>
                  <DESC>ACCT1
                  <INVACCTINFO>
                  <INVACCTFROM>
                  <BROKERID>vanguard.com
                  <ACCTID>1
                  </INVACCTFROM>
                  <USPRODUCTTYPE>401K
                  <CHECKING>N
                  <SVCSTATUS>ACTIVE
                  <INVACCTTYPE>INDIVIDUAL
                  </INVACCTINFO>
                  </ACCTINFO>
                  <ACCTINFO>
                  <INVACCTINFO>
                  <INVACCTFROM>
                  <BROKERID>vanguard.com
                  <ACCTID>2
                  </INVACCTFROM>
                  <USPRODUCTTYPE>IRA
                  <CHECKING>N
                  <SVCSTATUS>ACTIVE
                  <INVACCTTYPE>INDIVIDUAL
                  </INVACCTINFO>
                  </ACCTINFO>
                  <ACCTINFO>
                  <INVACCTINFO>
                  <INVACCTFROM>
                  <BROKERID>vanguard.com
                  <ACCTID>3
                  </INVACCTFROM>
                  <USPRODUCTTYPE>IRA
                  <CHECKING>N
                  <SVCSTATUS>ACTIVE
                  <INVACCTTYPE>INDIVIDUAL
                  </INVACCTINFO>
                  </ACCTINFO>
                  <ACCTINFO>
                  <INVACCTINFO>
                  <INVACCTFROM>
                  <BROKERID>vanguard.com
                  <ACCTID>4
                  </INVACCTFROM>
                  <USPRODUCTTYPE>IRA
                  <CHECKING>N
                  <SVCSTATUS>ACTIVE
                  <INVACCTTYPE>INDIVIDUAL
                  </INVACCTINFO>
                  </ACCTINFO>
                  <ACCTINFO>
                  <INVACCTINFO>
                  <INVACCTFROM>
                  <BROKERID>vanguard.com
                  <ACCTID>5
                  </INVACCTFROM>
                  <USPRODUCTTYPE>IRA
                  <CHECKING>N
                  <SVCSTATUS>ACTIVE
                  <INVACCTTYPE>INDIVIDUAL
                  </INVACCTINFO>
                  </ACCTINFO>
                  </ACCTINFORS>
                  </ACCTINFOTRNRS>
                  </SIGNUPMSGSRSV1>
                  </OFX>)FAKE"; return *this; }
    // Fake investment
    if (0) { response_ = R"FAKE(OFXHEADER:100
            DATA:OFXSGML
            VERSION:102
            SECURITY:NONE
            ENCODING:USASCII
            CHARSET:1252
            COMPRESSION:NONE
            OLDFILEUID:NONE
            NEWFILEUID:20180321182302.000

            <OFX><SIGNONMSGSRSV1><SONRS><STATUS><CODE>0<SEVERITY>INFO<MESSAGE>Successful Sign On</STATUS><DTSERVER>20180321202323[-5:EST]<LANGUAGE>ENG<DTPROFUP>20140605083000<FI><ORG>Vanguard<FID>15103</FI><SESSCOOKIE>xx</SONRS></SIGNONMSGSRSV1><INVSTMTMSGSRSV1><INVSTMTTRNRS><TRNUID>20180321182302.000<STATUS><CODE>0<SEVERITY>INFO</STATUS><CLTCOOKIE>4<INVSTMTRS><DTASOF>20180321160000.000[-5:EST]<CURDEF>USD<INVACCTFROM><BROKERID>vanguard.com<ACCTID>123</INVACCTFROM><INVTRANLIST><DTSTART>20160921160000.000[-5:EST]<DTEND>20180321202323.000[-5:EST]<BUYMF><INVBUY><INVTRAN><FITID>88032745229.5132.12212016.0<DTTRADE>20161221160000.000[-5:EST]<DTSETTLE>20161221160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>921937702<UNIQUEIDTYPE>CUSIP</SECID><UNITS>190.385<UNITPRICE>10.4<TOTAL>-1980.0<SUBACCTSEC>CASH<SUBAC
            CTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.5132.01302017.0<DTTRADE>20170130160000.000[-5:EST]<DTSETTLE>20170130160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>921937702<UNIQUEIDTYPE>CUSIP</SECID><UNITS>671.141<UNITPRICE>10.43<TOTAL>-7000.0<SUBACCTSEC>CASH<SUBACCTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.5132.01302017.1<DTTRADE>20170130160000.000[-5:EST]<DTSETTLE>20170130160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>921937702<UNIQUEIDTYPE>CUSIP</SECID><UNITS>287.632<UNITPRICE>10.43<TOTAL>-3000.0<SUBACCTSEC>CASH<SUBACCTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.0569.12212016.0<DTTRADE>20161221160000.000[-5:EST]<DTSETTLE>20161221160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>921909818<UNIQUEIDTYPE>CUSIP</SECID><UNITS>143.615<UNITPRICE>24.51<TOTAL>-3520.0<SUBACCTSEC>CASH<SUBACCTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.0569.12212016.1<DTTRADE>20161221160000.000[
            -5:EST]<DTSETTLE>20161221160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>921909818<UNIQUEIDTYPE>CUSIP</SECID><UNITS>0.004<UNITPRICE>24.51<TOTAL>-0.11<SUBACCTSEC>CASH<SUBACCTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.0569.01162018.0<DTTRADE>20180116160000.000[-5:EST]<DTSETTLE>20180116160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>921909818<UNIQUEIDTYPE>CUSIP</SECID><UNITS>136.292<UNITPRICE>31.88<TOTAL>-4345.0<SUBACCTSEC>CASH<SUBACCTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.0585.01162018.0<DTTRADE>20180116160000.000[-5:EST]<DTSETTLE>20180116160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>922908728<UNIQUEIDTYPE>CUSIP</SECID><UNITS>16.698<UNITPRICE>69.17<TOTAL>-1155.0<SUBACCTSEC>CASH<SUBACCTFUND>OTHER</INVBUY><BUYTYPE>BUY</BUYMF><BUYMF><INVBUY><INVTRAN><FITID>88032745229.0585.01162018.1<DTTRADE>20180116160000.000[-5:EST]<DTSETTLE>20180116160000.000[-5:EST]</INVTRAN><SECID><UNIQUEID>922908728<UNIQUEIDTYPE>CUSIP</SECID><UNITS>0.022<UNITPRICE>69.)FAKE"; return *this; }
  if (vars_map_.find("URL") == vars_map_.end()) {
    error_string_ = "URL not in vars map";
    return *this;
  }
  if (request_template_.empty()) {
    error_string_ = "no request template was added";
    return *this;
  }

  CURL *curl = curl_easy_init();
  if (!curl) {
    error_string_ = "Could not initialize curl";
    return *this;
  }

  string request_str = request();
  curl_easy_setopt(curl, CURLOPT_URL, vars_map_["URL"].c_str());
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str.c_str());

  struct curl_slist *headerlist = NULL;
  headerlist = curl_slist_append(headerlist, "Content-type: application/x-ofx");
  headerlist = curl_slist_append(headerlist, "Accept: */*, application/x-ofx");

  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteToString);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*) this);
  curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);

  CURLcode res = curl_easy_perform(curl);

  curl_easy_cleanup(curl);
  curl_slist_free_all(headerlist);

  if (res != CURLE_OK) {
    error_string_ = "Curl error: " + std::to_string(res);
    return *this;
  }
  return *this;
}

static size_t CurlWriteToString(char *ptr, size_t size, size_t nmemb, void *userdata) {
  OfxGetContext* context = static_cast<OfxGetContext*>(userdata);
  if (size != 1) {
    char err[255];
    sprintf(err, "In response, expected char size 1, got %ld", size);
    context->error_string_ = string(err);
  }
  string to_add;
  to_add.resize(nmemb);
  memcpy((void*) to_add.c_str(), (void*) ptr, nmemb);
  context->response_ += to_add;
  return size * nmemb;
}

size_t HeaderCallback(char *buffer, size_t size, size_t nitems,
                      void *userdata) {
  cout << "Read header: " << buffer;
  return nitems * size;
}




string OfxDate() {
  time_t now = time(nullptr);
  struct tm* ltime = localtime(&now);
  char time_str[255];
  sprintf(time_str, "%04d%02d%02d%02d%02d%02d.000",
          1900 + ltime->tm_year, 1 + ltime->tm_mon, ltime->tm_mday,
          ltime->tm_hour, ltime->tm_min, ltime->tm_sec);
  return time_str;
}

void InitVars(VarsMap* vars) {
  vars->clear();
  (*vars)["OFXDATE"] = OfxDate();
}


vector<string> GetMissingRequestVars(const string& request_template,
                                     const VarsMap& vars) {
  vector<string> missing;
  for (std::size_t i = 0; i < request_template.size(); i++) {
    if (request_template[i] == '$') {
      std::size_t j;
      for (j = i + 1; j < request_template.size(); j++) {
        if (!isupper(request_template[j])) {
          break;
        }
      }
      string var = request_template.substr(i + 1, j - i - 1);
      VarsMap::const_iterator it = vars.find(var);
      if (it == vars.end() || it->second.empty()) {
        missing.push_back(var);
      }
    }
  }
  return missing;
}

} // namespace: ofxget
