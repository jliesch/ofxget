#ifndef __CMDLINE_HH__
#define __CMDLINE_HH__

#include <cmdarg.hh>

class CmdLine
{
  CmdArgList  _cmdList;
  const char* _progName;
  int         _maxLength;
  
public:
  CmdLine(char*, CmdArg* ...);

  void usage();
  void parse(int argc, char* argv[]);
};

#endif // __CMDLINE_HH__
