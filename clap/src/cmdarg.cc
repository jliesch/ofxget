#include <iostream>
#include <string>

#include <cmdarg.hh>

using std::cerr;
using std::endl;

/*******************************************************************************
 *
 *                        Implementation on CmdArg
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArg
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *  Description: constructor
 *****************************************************************************/              
CmdArg::CmdArg(const char   optChar,
	       const char*  keyword,
	       const char*  valueName,
	       const char*  description,
	       unsigned int syntaxFlags) :
  _optChar(optChar),
  _keyword(keyword),
  _valueName(valueName),
  _description(description),
  _syntaxFlags(syntaxFlags),
  _status(isBAD)
{
  validate_flags();
}

/******************************************************************************
 *  Function   : ~CmdArg
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArg::~CmdArg() {}

/******************************************************************************
 *  service functions of CmdArg
 *****************************************************************************/
void         CmdArg::setValueName  (char* v){ _valueName = strdup(v);           }
void         CmdArg::setDescription(char* v){ _description = strdup(v);         }
const char   CmdArg::getOptChar    () const { return _optChar;                  }
const char*  CmdArg::getKeyword    () const { return _keyword;                  }
const char*  CmdArg::getValueName  () const { return _valueName;                }
const char*  CmdArg::getDescription() const { return _description;              }
unsigned int CmdArg::getSyntaxFlags() const { return _syntaxFlags;              }
bool         CmdArg::isHidden      () const { return (_syntaxFlags & isHIDDEN); }
bool         CmdArg::isOpt         () const { return (_syntaxFlags & isOPT);    }
bool         CmdArg::isValOpt      () const { return (_syntaxFlags & isVALOPT); }
bool         CmdArg::isBad         () const { return _status;                   }
void         CmdArg::setFound      ()       { _status |= isFOUND;               }
bool         CmdArg::isFound       () const { return (_status & isFOUND);       }
void         CmdArg::setValFound   ()       { _status |= isVALFOUND;            }
bool         CmdArg::isValFound    () const { return (_status & isVALFOUND);    }
void         CmdArg::setParseOK    ()       { _status |= isPARSEOK;             }
bool         CmdArg::isParseOK     () const { return (_status & isPARSEOK);     }
bool         CmdArg::isNull        () const { return !isParseOK();              }

/******************************************************************************
 *  Function   : validate_flags
 *  Args       : None
 *  Description: checks the validity of the syntax of a CmdArg and fix it.
 *               only one of isOPT, isREQ (default = isREQ).
 *               only one of isVALOPT, isVALREQ (default = isVALREQ)
 *  Returns    : void
 *****************************************************************************/              
bool CmdArg::validate_flags()
{
  if ((_syntaxFlags & isOPT) && (_syntaxFlags & isREQ))
    {
      cerr << "Warning: keyword " << getKeyword()
	   << " can't be optional AND required" << endl;
      cerr << "changing the sytax of "
	   << getKeyword() << " to be required." << endl;
      _syntaxFlags &= ~isOPT;
      return false;
    }
  if ((_syntaxFlags & isVALOPT) && (_syntaxFlags & isVALREQ  ))
    {
      cerr << "Warning: value for keyword " << getKeyword()
	   << " can't be optional AND required" << endl;
      cerr << "changing the sytax of the value of "
	   << getKeyword() << " to be required." << endl;
      _syntaxFlags &= ~isVALREQ;
      return false;
    }
  return true;
}

/*******************************************************************************
 *
 *                        Implementation on CmdArgInt
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgInt
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               def         - default value.
 *  Description: constructor
 *****************************************************************************/              
CmdArgInt::CmdArgInt(const char   optChar,
		     const char*  keyword,
		     const char*  valueName,
		     const char*  description,
		     unsigned int syntaxFlags,
		     int          def) :
  CmdArg(optChar, keyword, valueName, description, syntaxFlags), _v(def)
{}

/******************************************************************************
 *  Function   : ~CmdArgInt
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgInt::~CmdArgInt() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the integer pointed by i++
 *  Returns    : true if the value evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgInt::getValue(int *i, int argc, char *argv[])
{
  char *ptr;
  (*i)++;
  if (*i < argc)
    {
      char* arg = strdup(argv[*i]);
      _v = strtol(arg, (char **)&ptr, 0);
      if (ptr == arg)
	{
	  cout << " invalid integer value \"" << arg << "\"" << endl;
	  return false;
	}
      setParseOK();
      return true;
    }
  else
    return false;  
}

/******************************************************************************
 *  Function   : operator int
 *  Args       : None
 *  Description: casting operator to int.
 *  Returns    : the int value
 *****************************************************************************/
CmdArgInt::operator int() { return _v; }

ostream& operator<<(ostream &os, const CmdArgInt& cmd)
{ return os << cmd._v; }



/*******************************************************************************
 *
 *                        Implementation on CmdArgFloat
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgFloat
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               def         - default value.
 *  Description: constructor
 *****************************************************************************/              
CmdArgFloat::CmdArgFloat(const char   optChar,
			 const char*  keyword,
			 const char*  valueName,
			 const char*  description,
			 unsigned int syntaxFlags,
			 double       def) :
  CmdArg(optChar, keyword, valueName, description, syntaxFlags), _v(def)
{}

/******************************************************************************
 *  Function   : ~CmdArgFloat
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgFloat::~CmdArgFloat() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the float-number pointed by i++
 *  Returns    : true if the value evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgFloat::getValue(int *i, int argc, char *argv[])
{
  char *ptr;
  (*i)++;
  if (*i < argc)
    {
      char* arg = strdup(argv[*i]);
      _v = strtod(arg, (char **)&ptr);
      if (ptr == arg)
	{
	  cout << " invalid float value \"" << arg << "\"" << endl;
	  return false;
	}
      setParseOK();
      return true;
    }
  else
    return false;  
}

/******************************************************************************
 *  Function   : operator float
 *  Args       : None
 *  Description: casting operator to float.
 *  Returns    : the float value
 *****************************************************************************/
CmdArgFloat::operator float() { return _v; }

ostream& operator<<(ostream &os, const CmdArgFloat& cmd)
{ return os << cmd._v; }


/*******************************************************************************
 *
 *                        Implementation on CmdArgBool
 *
 ******************************************************************************/


/******************************************************************************
 *  Function   : CmdArgBool
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *  Description: constructor
 *****************************************************************************/              
CmdArgBool::CmdArgBool(const char   optChar,
		       const char*  keyword,
		       const char*  description,
		       unsigned int syntaxFlags) :
  CmdArg(optChar, keyword, "", description, syntaxFlags), _v(false)
{}

/******************************************************************************
 *  Function   : ~CmdArgBool
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgBool::~CmdArgBool() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : int*,int,char**
 *  Description: set the internal value to be true, return true.
 *  Returns    : true.
 *****************************************************************************/              
bool CmdArgBool::getValue(int *, int, char**)
{
  _v = true;
  setParseOK();
  return true;
}

/******************************************************************************
 *  Function   : operator bool
 *  Args       : None
 *  Description: casting operator to bool.
 *  Returns    : the bool value
 *****************************************************************************/
CmdArgBool::operator bool() { return _v; }

ostream& operator<<(ostream &os, const CmdArgBool& cmd)
{ return os << cmd._v; }


/*******************************************************************************
 *
 *                        Implementation on CmdArgStr
 *
 ******************************************************************************/


/******************************************************************************
 *  Function   : CmdArgStr
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               def         - default value
 *  Description: constructor
 *****************************************************************************/              
CmdArgStr::CmdArgStr(const char   optChar,
		     const char*  keyword,
		     const char*  valueName,
		     const char*  description,
		     unsigned int syntaxFlags,
		     char*        def) :
  CmdArg(optChar, keyword, valueName, description, syntaxFlags), _v(def)
{}

/******************************************************************************
 *  Function   : ~CmdArgStr
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgStr::~CmdArgStr()
{ if (_v) delete _v; }

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the string pointed by i++
 *  Returns    : true if the string evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgStr::getValue(int *i, int argc, char *argv[])
{
  (*i)++;
  if (*i < argc)
    {
      char* arg = strdup(argv[*i]);
      if (arg[0] == '-') return false;      
      _v = arg;
      setParseOK();
      return true;
    }
  else
    return false;  
}

/******************************************************************************
 *  Function   : operator char*
 *  Args       : None
 *  Description: casting operator to char*.
 *  Returns    : the char* value
 *****************************************************************************/
CmdArgStr::operator char*() {
  char* str = new char[strlen(_v) + 1];
  int i;
  for (i = 0; i < strlen(_v); i++)
    str[i] = _v[i];
  str[i] = '\0';
  return str;
}

ostream& operator<<(ostream &os, const CmdArgStr& cmd)
{ return os << cmd._v; }


/*******************************************************************************
 *
 *                        Implementation on CmdArgChar
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgChar
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               def         - default value
 *  Description: constructor
 *****************************************************************************/              
CmdArgChar::CmdArgChar(const char   optChar,
		       const char*  keyword,
		       const char*  valueName,
		       const char*  description,
		       unsigned int syntaxFlags,
		       char         def) :
  CmdArg(optChar, keyword, valueName, description, syntaxFlags), _v(def)
{}

/******************************************************************************
 *  Function   : ~CmdArgChar
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgChar::~CmdArgChar() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the string pointed by i++
 *  Returns    : true if the string evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgChar::getValue(int *i, int argc, char *argv[])
{
  (*i)++;
  if (*i < argc)
    {
      char* arg = strdup(argv[*i]);
      if (strlen(arg) > 1){
	cout << "value \"" << arg << "\" is to long. ignoring" << endl;
	return false;
      }
      _v = arg[0];
      setParseOK();
      return true;
    }
  else
    return false;  
}

/******************************************************************************
 *  Function   : operator char
 *  Args       : None
 *  Description: casting operator to char.
 *  Returns    : the first char of the value
 *****************************************************************************/
CmdArgChar::operator char() {
  return _v;
}

ostream& operator<<(ostream &os, const CmdArgChar& cmd)
{ return os << cmd._v; }



/*******************************************************************************
 *
 *                        Implementation on CmdArgIntList
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgIntList
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               delim       - delimiters.
 *               minSize     - minimum list size.
 *               maxSize     - maximum list size.
 *  Description: constructor
 *****************************************************************************/
CmdArgIntList::CmdArgIntList(const char   optChar,
			     const char*  keyword,
			     const char*  valueName,
			     const char*  description,
			     unsigned int syntaxFlags,
			     int          minSize,
			     int          maxSize,
			     const char*  delim) :
  CmdArgTypeList<int>(optChar, keyword, valueName, description, syntaxFlags,
		      minSize, maxSize, delim)
{}

/******************************************************************************
 *  Function   : ~CmdArgIntList
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgIntList::~CmdArgIntList() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the integer pointed by i++
 *  Returns    : true if the value evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgIntList::getValue(int *i, int argc, char *argv[])
{
  char *ptr;
  long val;
  (*i)++;
  if (*i < argc)
    {
      char* tokens = strdup(argv[*i]);
      char* tok    = strtok(tokens, getDelimiters());
      while (tok)
	{
	  val = strtol(tok, (char **)&ptr, 0);
	  if (ptr == tok)
	    {
	      cout << "invalid integer value \"" << tok << "\"" << endl;
	      return false;
	    }
	  insert(val);
	  tok = strtok(NULL, getDelimiters());
	}
      return validate();
    }
  else
    return false;  
}

ostream& operator<<(ostream& os, CmdArgIntList& cmd)
{
  return os << cmd;
}


/*******************************************************************************
 *
 *                        Implementation on CmdArgFloatList
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgFloatList
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               delim       - delimiters.
 *               minSize     - minimum list size.
 *               maxSize     - maximum list size.
 *  Description: constructor
 *****************************************************************************/
CmdArgFloatList::CmdArgFloatList(const char   optChar,
				 const char*  keyword,
			         const char*  valueName,
				 const char*  description,
				 unsigned int syntaxFlags,
				 int          minSize,
				 int          maxSize,
				 const char*  delim) :
  CmdArgTypeList<double>(optChar, keyword, valueName, description, syntaxFlags,
			 minSize, maxSize, delim)
{}

/******************************************************************************
 *  Function   : ~CmdArgFloatList
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgFloatList::~CmdArgFloatList() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the float pointed by i++
 *  Returns    : true if the value evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgFloatList::getValue(int *i, int argc, char *argv[])
{
  char *ptr;
  double val;
  (*i)++;
  if (*i < argc)
    {
      char* tokens = strdup(argv[*i]);
      char* tok    = strtok(tokens, getDelimiters());
      while (tok)
	{
	  val = strtod(tok, (char **)&ptr);
	  if (ptr == tok)
	    {
	      cout << "invalid float value \"" << tok << "\"" << endl;
	      return false;
	    }
	  insert(val);
	  tok = strtok(NULL, getDelimiters());
	}
      return validate();
    }
  else
    return false;  
}

ostream& operator<<(ostream& os, CmdArgFloatList& cmd)
{
  return os << cmd;
}


/*******************************************************************************
 *
 *                        Implementation on CmdArgStrList
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgStrList
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               delim       - delimiters.
 *               minSize     - minimum list size.
 *               maxSize     - maximum list size.
 *  Description: constructor
 *****************************************************************************/
CmdArgStrList::CmdArgStrList(const char   optChar,
			     const char*  keyword,
			     const char*  valueName,
			     const char*  description,
			     unsigned int syntaxFlags,
			     int          minSize,
			     int          maxSize,
			     const char*  delim) :
  CmdArgTypeList<char*>(optChar, keyword, valueName, description, syntaxFlags,
			minSize, maxSize, delim)
{}

/******************************************************************************
 *  Function   : ~CmdArgStrList
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgStrList::~CmdArgStrList() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the integer pointed by i++
 *  Returns    : true if the value evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgStrList::getValue(int *i, int argc, char *argv[])
{
  char *ptr;
  (*i)++;
  if (*i < argc)
    {
      char* tokens = strdup(argv[*i]);
      char* tok    = strtok(tokens, getDelimiters());
      while (tok)
	{
	  insert(strdup(tok));
	  tok = strtok(NULL, getDelimiters());
	}
      return validate();
    }
  else
    return false;  
}

ostream& operator<<(ostream& os, CmdArgStrList& cmd)
{
  return os << cmd;
}


/*******************************************************************************
 *
 *                        Implementation on CmdArgCharList
 *
 ******************************************************************************/

/******************************************************************************
 *  Function   : CmdArgCharList
 *  Args       : optChar     - optional char instead of keyword
 *               keyword     - the keyword expected in the command line
 *		 valueName   - the name of the vale (if required)
 *		 description - description to be printed in usage()
 *		 syntaxFlags - the syntax of the argument where:
 *                             isREQ, isOPT - declare the if the keyword
 *                             required or not.
 *                             isVALOPT, isVALREQ - declare if the keyword
 *                             needs a value.
 *                             isHIDDEN - if set, the keyword won't show in
 *                             usage().
 *               delim       - delimiters.
 *               minSize     - minimum list size.
 *               maxSize     - maximum list size.
 *  Description: constructor
 *****************************************************************************/
CmdArgCharList::CmdArgCharList(const char   optChar,
			       const char*  keyword,
			       const char*  valueName,
			       const char*  description,
			       unsigned int syntaxFlags,
			       int          minSize,
			       int          maxSize,
			       const char*  delim) :
  CmdArgTypeList<char>(optChar, keyword, valueName, description, syntaxFlags,
		       minSize, maxSize, delim)
{}

/******************************************************************************
 *  Function   : ~CmdArgCharList
 *  Args       : None
 *  Description: destructor
 *****************************************************************************/              
CmdArgCharList::~CmdArgCharList() {}

/******************************************************************************
 *  Function   : getValue
 *  Args       : i    - pointer to the index in argv
 *               argc - as passed from main
 *               argv - as passed from main
 *  Description: try to gets the value of the integer pointed by i++
 *  Returns    : true if the value evalueted successfully, false otherwise.
 *****************************************************************************/              
bool CmdArgCharList::getValue(int *i, int argc, char *argv[])
{
  char *ptr;
  (*i)++;
  if (*i < argc)
    {
      char* tokens = strdup(argv[*i]);
      char* tok    = strtok(tokens, getDelimiters());
      while (tok)
	{
	  if (strlen(tok) != 1)
	    {
	      cout << "invalid char value \"" << tok << "\"" << endl;
	      return false;
	    }
	  insert(tok[0]);
	  tok = strtok(NULL, getDelimiters());
	}
      return validate();
    }
  else
    return false;  
}

ostream& operator<<(ostream& os, CmdArgCharList& cmd)
{
  return os << cmd;
}
