#ifndef __CMDARG_HH__
#define __CMDARG_HH__

#include <iostream>
#include <list>
#include <string.h>

using std::cout;
using std::endl;
using std::list;
using std::ostream;

class CmdLine;

/*******************************************************************************
 *
 *                           Definition of class CmdArg
 *
 ******************************************************************************/
class CmdArg
{
  const char    _optChar;
  const char   *_keyword;
  const char   *_valueName; // JDL: changed to const
  const char   *_description; // JDL: changed to const
  unsigned int  _syntaxFlags;
  unsigned int  _status;
  
  friend CmdLine;

  enum e_CmdArgStatus{
    isBAD       = 0x00,  // bad status
    isFOUND     = 0x01,  // argument was found in the command line
    isVALFOUND  = 0x02,  // the value of the argument was found in the command line
    isPARSEOK   = 0x04   // argument was found and its value is ok
  };

protected:
  void setValueName(char*);
  void setDescription(char*);
  
public:
  enum e_CmdArgSyntax{
    isOPT       = 0x01,  // argument is optional
    isREQ       = 0x02,  // argument is required
    isVALOPT    = 0x04,  // argument value is optional
    isVALREQ    = 0x08,  // argument value is required
    isHIDDEN    = 0x10   // argument is not to be printed in usage    
  };
  
  CmdArg(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ));
  virtual ~CmdArg();

  // selectors
  const char   getOptChar    () const;
  const char*  getKeyword    () const;
  const char*  getValueName  () const;
  const char*  getDescription() const;
  unsigned int getSyntaxFlags() const;
  
  bool         isHidden      () const;
  bool         isOpt         () const;
  bool         isValOpt      () const;
  
  bool         isBad         () const;
  void         setFound      ();
  bool         isFound       () const;
  void         setValFound   ();
  bool         isValFound    () const;
  void         setParseOK    ();
  bool         isParseOK     () const;

  // methods
  bool         isNull        () const;
  virtual bool getValue      (int*,int,char**) = 0;
  bool         validate_flags();
};


// definition of Arglist
typedef list<CmdArg*> CmdArgList;
typedef list<CmdArg*>::iterator CmdArgListIterator;


/*******************************************************************************
 *
 *                         Definition of class CmdArgInt
 *
 ******************************************************************************/
class CmdArgInt : public CmdArg
{
  long _v;

public:
  CmdArgInt(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
	    int = 0);
  ~CmdArgInt();

  virtual bool getValue(int*,int,char**);
  operator int();
  friend ostream& operator<<(ostream&, const CmdArgInt&);
};


/*******************************************************************************
 *
 *                         Definition of class CmdArgFloat
 *
 ******************************************************************************/
class CmdArgFloat : public CmdArg
{
  double _v;

public:
  CmdArgFloat(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
	      double = 0);
  ~CmdArgFloat();
  
  virtual bool getValue(int*,int,char**);
  operator float();
  friend ostream& operator<<(ostream&, const CmdArgFloat&);
};


/*******************************************************************************
 *
 *                         Definition of class CmdArgBool
 *
 ******************************************************************************/
class CmdArgBool : public CmdArg
{
  bool _v;

public:
  CmdArgBool(const char, const char*, const char*, unsigned int = (isREQ | isVALREQ));
  ~CmdArgBool();

  virtual bool getValue(int*,int,char**);
  operator bool();
  friend ostream& operator<<(ostream&, const CmdArgBool&);
};


/*******************************************************************************
 *
 *                         Definition of class CmdArgStr
 *
 ******************************************************************************/
class CmdArgStr : public CmdArg
{
  char* _v;

public:
  CmdArgStr(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
	    char* = NULL);
  ~CmdArgStr();

  virtual bool getValue(int*,int,char**);
  operator char*();
  friend ostream& operator<<(ostream&, const CmdArgStr&);
};


/*******************************************************************************
 *
 *                         Definition of class CmdArgChar
 *
 ******************************************************************************/
class CmdArgChar : public CmdArg
{
  char _v;

public:
  CmdArgChar(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
	     char = '\0');
  ~CmdArgChar();
  
  virtual bool getValue(int*,int,char**);
  operator char();
  friend ostream& operator<<(ostream&, const CmdArgChar&);
};


/*******************************************************************************
 *
 *                         Definition of class CmdArgList
 *
 ******************************************************************************/
template <class T>
class CmdArgTypeList : public CmdArg
{
public:
  typedef list<T>           ArgList;
  typedef typename ArgList::iterator ArgListIterator;
  
private:
  ArgList         _list;
  ArgListIterator _curr;
  int             _index;
  const char*     _delimiters;
  int             _max;
  int             _min;

public:
  CmdArgTypeList(const char   optChar,
		 const char*  keyword,
		 const char*  valueName,
		 const char*  description,
		 unsigned int syntaxFlags = (isREQ | isVALREQ),
		 int          minSize = 1,
		 int          maxSize = 100,
		 const char*  delim = ",-~/.")
    :   CmdArg(optChar, keyword, valueName, description, syntaxFlags),
	_delimiters(delim), _min(minSize),_max(maxSize)    
  {
    char buf[512];
    sprintf(buf, "%s1%c...%c%sn", getValueName(), _delimiters[0],
	    _delimiters[0], getValueName());
    setValueName(buf);
    sprintf(buf, "%s (%d <= n <= %d)", description, _min, _max);
    setDescription(buf);
    for (int i = 0; i < strlen(delim); i++){
      if (delim[i] == ' '){
	cout << "ERROR: space can't be a delimiter" << endl;
	exit(0);}}
  }
  ~CmdArgTypeList() {}
  
  const char*  getDelimiters() { return _delimiters;}
  int          getMaxSize   () { return _max; }
  int          getMinSize   () { return _min; }
  virtual bool getValue     (int*,int,char**) = 0;
  
  T operator[](int idx) {
    idx = idx % _list.size();
    while (_index != idx)
      if (_index < idx){ _index++; _curr++; }
      else { _index--; _curr--; }
    return *_curr;    
  }
  virtual void reset        () { _curr  = _list.begin(); _index = 0; }
  int          size         () { return _list.size(); }
  void         insert       (T item){ if (_list.size() < _max) _list.push_back(item); }
  bool         validate     (){
    if (_list.size() < _min) {
      cout << "too few argument to the switch -" << getKeyword() << endl;
      return false;
    }
    if (_list.size() > _max) {
      cout << "too many argument to the switch -" << getKeyword() << endl;
      return false;
    }
    reset();
    setParseOK();
    return true;    
  }
  
  friend ostream& operator<<(ostream&, const CmdArgTypeList<T>&);
  ostream& print_me(ostream&) const;
};

template <class T>
ostream& CmdArgTypeList<T>::print_me(ostream &os) const
{
  typename ArgList::const_iterator it;
  it = _list.begin();
  os << *it;
  it++;
  for (;it != _list.end(); it++) {
    os << _delimiters[0] << *it;
  }
  return os;
}

template <class T>
ostream& operator<<(ostream &os, const CmdArgTypeList<T>& cmd)
{
  return cmd.print_me(os);
}

/*******************************************************************************
 *
 *                         Definition of class CmdArgIntList
 *
 ******************************************************************************/
class CmdArgIntList : public CmdArgTypeList<int>
{
public:
  CmdArgIntList(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
		int = 1, int = 100, const char* = ",~/");
  ~CmdArgIntList();

  virtual bool getValue(int*, int, char**);
};


/*******************************************************************************
 *
 *                     Definition of class CmdArgFloatList
 *
 ******************************************************************************/
class CmdArgFloatList : public CmdArgTypeList<double>
{
public:
  CmdArgFloatList(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
		  int = 1, int = 100, const char* = ",~/");
  ~CmdArgFloatList();

  virtual bool getValue(int*, int, char**);
};


/*******************************************************************************
 *
 *                     Definition of class CmdArgStrList
 *
 ******************************************************************************/
class CmdArgStrList : public CmdArgTypeList<char*>
{
public:
  CmdArgStrList(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
		int = 1, int = 100, const char* = ",~/.-");
  ~CmdArgStrList();

  virtual bool getValue(int*, int, char**);
};


/*******************************************************************************
 *
 *                     Definition of class CmdArgCharList
 *
 ******************************************************************************/
class CmdArgCharList : public CmdArgTypeList<char>
{
public:
  CmdArgCharList(const char, const char*, const char*, const char*, unsigned int = (isREQ | isVALREQ),
		 int = 1, int = 100, const char* = ",~/.-");
  ~CmdArgCharList();

  virtual bool getValue(int*, int, char**);
};

#endif // __CMDARG_HH__
