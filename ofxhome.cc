#include <iostream>
#include <regex>
#include <curl/curl.h>

#include "pugixml/pugixml.hpp"

#include "ofxhome.h"

using ofxget::Institution;
using std::cout;
using std::endl;
using std::regex_constants::icase;
using std::string;

namespace ofxget {

static size_t CurlWriteToString(char *ptr, size_t size, size_t nmemb, void *userdata) {
  if (size != 1) {
    char err[255];
    sprintf(err, "In response, expected char size 1, got %ld", size);
    throw string(err);
  }
  string* out = static_cast<string*>(userdata);
  string to_add;
  to_add.resize(nmemb);
  memcpy((void*) to_add.c_str(), (void*) ptr, nmemb);
  *out += to_add;
  return size * nmemb;
}

string OfxHomeFullDumpString() {
    // Fake debugging response
    if (false) {
        return R"RESPONSE(<?xml version="1.0" encoding="utf-8"?>
                <institution id="421">
                <name>ING DIRECT (Canada)</name>
                <fid>061400152</fid>
                <org>INGDirectCanada</org>
                <url>https://ofx.ingdirect.ca</url>
                <ofxfail>1</ofxfail>
                <sslfail>5</sslfail>
                <lastofxvalidation>2014-10-26 22:57:57</lastofxvalidation>
                <lastsslvalidation>2014-12-03 23:01:03</lastsslvalidation>
                <profile addr1="3389 Steeles Avenue East" city="Toronto" state="ON" postalcode="M2H 3S8" country="CAN" csphone="800-464-3473" tsphone="800-464-3473" url="http://www.tangerine.ca" email="clientservices@ingdirect.ca" signonmsgset="true" bankmsgset="true"/>
                </institution>
                <institution id="422">
                <name>Safe Credit Union - OFX Beta</name>
                <fid>321173742</fid>
                <org>DI</org><url>https://ofxcert.diginsite.com/cmr/cmr.ofx</url>
                <ofxfail>3</ofxfail>
                <sslfail>0</sslfail>
                <lastofxvalidation>2010-03-18 16:25:00</lastofxvalidation>
                <lastsslvalidation>2018-03-17 00:12:01</lastsslvalidation>
                </institution>

<institution id="479">
<name>Vanguard Group, The</name>
<fid>15103</fid>
<org>Vanguard</org>
<brokerid>vanguard.com</brokerid>
<url>https://vesnc.vanguard.com/us/OfxDirectConnectServlet</url>
<ofxfail>0</ofxfail>
<sslfail>0</sslfail>
<lastofxvalidation>2018-03-18 00:34:54</lastofxvalidation>
<lastsslvalidation>2018-03-18 00:34:54</lastsslvalidation>
<profile finame="Vanguard" addr1="P.O. Box 1110" city="Valley Forge" state="PA" postalcode="19482-1110" country="USA" csphone="1-212-723-2898" tsphone="1-212-723-2898" url="https://vesnc.vanguard.com/us/OfxDirectConnectServlet" email="alex_shnir@smb.com" signonmsgset="true" bankmsgset="true" invstmtmsgset="true" emailmsgset="true" seclistmsgset="true"/>
</institution>

)RESPONSE";
    }
    CURL *curl = curl_easy_init();
    if (! curl) {
      throw string("Could not initialize curl");
    }

    string response;
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.ofxhome.com/api.php?dump=yes");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw "Curl error: " + std::to_string(res);
    }

    curl_easy_cleanup(curl);

    return response;
}

std::ostream& operator<<(std::ostream& stream,
                         const Institution& inst) {
  stream << "OFXHome id: " << inst.ofxhome_id << endl;
  stream << "Name      : " << inst.name << endl;
  stream << "URL       : " << inst.url << endl;
  stream << "FID       : " << inst.fid << endl;
  stream << "ORG       : " << inst.org << endl;
  if (!inst.brokerid.empty()) {
    stream << "BROKERID  : " << inst.brokerid << endl;
  }
  if (!inst.bankid.empty()) {
    stream << "BANKID    : " << inst.bankid << endl;
  }
  stream << "OFX Fail  : " << inst.ofxfail << endl;
  stream << "SSL Fail  : " << inst.sslfail << endl;
  stream << "OFX Last  : " << inst.lastofxvalidation << endl;
  stream << "SSL Last  : " << inst.lastsslvalidation << endl;
  for (const auto& p : inst.profile) {
    stream << "Profile   : " << p.first << "=" << p.second << endl;
  }
  return stream;
}

vector<Institution> OfxDumpStringToInstitutions(const string& dump) {
  pugi::xml_document doc;
  pugi::xml_parse_result result = doc.load_string(dump.c_str());
  if (!result) {
    throw string("Could not parse institutions");
  }

  vector<Institution> insts;
  for (pugi::xml_node inst = doc.child("institution"); inst; inst = inst.next_sibling("institution")) {
    Institution i;
    i.ofxhome_id = inst.attribute("id").as_int();
    i.name = inst.child("name").child_value();
    i.url = inst.child("url").child_value();
    i.fid = inst.child("fid").child_value();
    i.org = inst.child("org").child_value();
    i.brokerid = inst.child("brokerid").child_value();
    i.bankid = inst.child("bankid").child_value();
    i.ofxfail = inst.child("ofxfail").text().as_int();
    i.sslfail = inst.child("sslfail").text().as_int();
    i.lastofxvalidation = inst.child("lastofxvalidation").child_value();
    i.lastsslvalidation = inst.child("lastsslvalidation").child_value();
    auto profile = inst.child("profile");
    for (pugi::xml_attribute_iterator ait = profile.attributes_begin(); ait != profile.attributes_end(); ++ait)
    {
      i.profile[ait->name()] = ait->value();
    }
    insts.push_back(i);
  }

  return insts;
}

const Institution* FindInstitutionByName(
    const vector<Institution>& institutions,
    const string& name)
{
  for (const Institution& i : institutions) {
    if (i.name.find(name) != string::npos) {
      return &i;
    }
  }
  return nullptr;
}

string AnonymizeRequest(const string& request) {
  string r = request;
  std::regex userid("<USERID>[^<]*", icase);
  std::regex acctid("<ACCTID>[^<]*", icase);
  std::regex userpass("<USERPASS>[^<]*", icase);
  r = regex_replace(r, userid, "<USERID>X\n");
  r = regex_replace(r, acctid, "<ACCTID>X\n");
  r = regex_replace(r, userpass, "<USERPASS>X\n");
  return r;
}

string UploadSuccessfulRequest(const string& api_key, const string& url, const string& request) {
    CURL *curl = curl_easy_init();
    if (! curl) {
      throw string("Could not initialize curl");
    }

    char *api_key_output = curl_easy_escape(curl, api_key.c_str(), api_key.size());
    string anonymized_request = AnonymizeRequest(request);
    char *request_output = curl_easy_escape(curl, anonymized_request.c_str(), anonymized_request.size());
    char *url_output = curl_easy_escape(curl, url.c_str(), url.size());
    if (!api_key_output || !request_output || !url_output) {
      throw string("Could not curl_easy_escape request");
    }
    string output = string("api_key=") + api_key_output + "&url=" + url_output + "&request=" + request_output;
    string response;
#ifdef OFXHOME_DEBUG
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/ofxhome/index.php/api/successfulRequest");
#else
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.ofxhome.com/beta/index.php/api/successfulRequest");
#endif
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (long) output.size());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw "Curl error: " + std::to_string(res);
    }

    curl_free(api_key_output);
    curl_free(request_output);
    curl_easy_cleanup(curl);

    return response;
}

// Returns true if the api_key is avalid.
bool ValidateApiKey(const string& api_key) {
    CURL *curl = curl_easy_init();
    if (! curl) {
      throw string("Could not initialize curl");
    }

    char *api_key_output = curl_easy_escape(curl, api_key.c_str(), api_key.size());
    string output = string("api_key=") + api_key_output;
    string response;
#ifdef OFXHOME_DEBUG
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/ofxhome/index.php/api/checkApiKey");
#else
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.ofxhome.com/beta/index.php/api/checkApiKey");
#endif
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteToString);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (long) output.size());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, output.c_str());

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        throw "Curl error: " + std::to_string(res);
    }

    curl_free(api_key_output);
    curl_easy_cleanup(curl);

    return response == "success";
}

} // namespace ofxget
