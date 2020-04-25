#include <cmdline.hh>
#include <stdarg.h>
//#include <minmax.h>
#include <stdio.h>
#include <iomanip>

using std::ios;
using std::setw;

/******************************************************************************
 *  Function   : CmdLine
 *  Args       : progName - the program name.
 *               cmds     - list of CmdArg.
 *  Description: constructor
 *****************************************************************************/              
CmdLine::CmdLine(char *progName, CmdArg *cmds ...) :
  _progName(progName), _maxLength(0)
{
  va_list args;
  va_start(args, cmds);
  for (CmdArg* arg = cmds; arg; arg = va_arg(args, CmdArg*))
    {
      _cmdList.push_front(arg);
      _maxLength = std::max((long)_maxLength, (long)strlen(arg->getValueName()));
    }
  va_end(args);
}

/******************************************************************************
 *  Function   : usage
 *  Args       : None
 *  Description: prints the usage of the program
 *  Returns    : void
 *****************************************************************************/              
void CmdLine::usage()
{
  CmdArg*            arg;
  CmdArgListIterator it;
  
  cout << endl << "Usage: " << _progName;
  for (it = _cmdList.begin(); it != _cmdList.end(); it++)
    {
      arg = *it;
      if (!arg->isHidden())
	{
	  cout << " ";
	  if (arg->isOpt()) cout << "[";
	  cout << "-" << arg->getKeyword();
	  if (!arg->isValOpt())
	    cout << " " << arg->getValueName();
	  if (arg->isOpt()) cout << "]";
	}
    }

  cout << endl << "Where: " << endl;
  for (it = _cmdList.begin(); it != _cmdList.end(); it++)
    {
      arg = *it;
      if (!arg->isHidden())
	{
	  cout << setw(_maxLength + 5);
	  if (!arg->isValOpt())	  
	    cout << arg->getValueName();
	  else
	    cout << arg->getKeyword(); // usualy boolean keyword
	  cout.flags(ios::left);
	  cout << " - " << arg->getDescription() << endl;
	  cout.flags(ios::right);
	}
    }
  cout << endl;
  exit(1);
}

/******************************************************************************
 *  Function   : parse
 *  Args       : argc, argv - exactly as taken from the command line
 *  Description: parse the command line, exits with usage if not fulfill all
 *               the required parameters.
 *  Returns    : void
 *****************************************************************************/              
void CmdLine::parse(int argc, char* argv[])
{
  CmdArg*            cmd;
  CmdArgListIterator it;

  for (int i = 1; i < argc; i++)
    {
      char* arg = argv[i];
      bool found = false;
      for (it = _cmdList.begin(); (it != _cmdList.end())&& !found; it++)
	{
	  cmd = *it;
	  char cmdWord[512], cmdChar[512];
	  sprintf(cmdWord, "-%s", cmd->getKeyword());
	  sprintf(cmdChar, "-%c", cmd->getOptChar());
	  if (!strcmp(arg, cmdWord) || !strcmp(arg, cmdChar))
	    {
	      cmd->setFound();
	      if (!cmd->getValue(&i, argc, argv))
		{
		  cout << "Error: switch -" << cmd->getKeyword()
		       << " must take an argument" << endl;
		  usage();
		}
	      else
		{
		  cmd->setValFound();
		}
	      found = true;
	    }
	}
      if (!found)
	{
	  cout << "Warning: argument \"" << arg
	       << "\" looks strange, ignoring." << endl;
	}
    }
  
  for (it = _cmdList.begin(); it != _cmdList.end(); it++)
    {
      cmd = *it;
      if (!cmd->isOpt()) // i.e required
	if (!cmd->isFound())
	  {
	    cout << "Error: the switch -" << cmd->getKeyword()
		 << " must be supplied" << endl;
	    usage();
	  }
      
      if (cmd->isFound() && !cmd->isValOpt()) // i.e need value
	if (!cmd->isValFound())
	  {
	    cout << "Error: the switch -" << cmd->getKeyword()
		 << " must take a vlue" << endl;
	    usage();
	  }
    }
}
