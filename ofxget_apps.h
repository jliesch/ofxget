namespace ofxget {

struct AppInfo {
  AppInfo(const string& n, const string& id, const string& ver) {
    name = n;
    appid = id;
    appver = ver;
  }

  string name;
  string appid;
  string appver;
};

std::vector<AppInfo> OfxApps() {
  std::vector<AppInfo> apps = {
    AppInfo("Money_2007", "MONEY", "1600"),
    AppInfo("Quicken_2005", "QWIN", "1400"),
    AppInfo("Quicken_2010", "QWIN", "1800"),
    AppInfo("Quicken_2011", "QWIN", "1900"),
    AppInfo("Quicken_2016", "QWIN", "2500"),
    AppInfo("QuickBooks_2008", "QBW", "1800")
  };
  return apps;
}

} // namespace: ofxget
