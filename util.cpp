#include "StdAfx.h"
#include "qtools/qdict.h"
#include "qtools/qstrlist.h"
#include "qtools/qregexp.h"
#include "util.h"
#include "growbuf.h"
#include "md5.h"
#include "portable.h"

//--------------------------------------------------------------------------

static QDict<int> g_extLookup;

static struct Lang2ExtMap
{
  const char *langName;
  const char *parserName;
  SrcLangExt parserId;
}
g_lang2extMap[] =
{
//  language       parser           parser option
  { "idl",         "c",             SrcLangExt_IDL      },
  { "java",        "c",             SrcLangExt_Java     },
  { "javascript",  "c",             SrcLangExt_JS       },
  { "csharp",      "c",             SrcLangExt_CSharp   },
  { "d",           "c",             SrcLangExt_D        },
  { "php",         "c",             SrcLangExt_PHP      },
  { "objective-c", "c",             SrcLangExt_ObjC     },
  { "c",           "c",             SrcLangExt_Cpp      },
  { "c++",         "c",             SrcLangExt_Cpp      },
  { "python",      "python",        SrcLangExt_Python   },
  { "fortran",     "fortran",       SrcLangExt_Fortran  },
  { "fortranfree", "fortranfree",   SrcLangExt_Fortran  },
  { "fortranfixed", "fortranfixed", SrcLangExt_Fortran  },
  { "vhdl",        "vhdl",          SrcLangExt_VHDL     },
  { "dbusxml",     "dbusxml",       SrcLangExt_XML      },
  { "tcl",         "tcl",           SrcLangExt_Tcl      },
  { "md",          "md",            SrcLangExt_Markdown },
  { 0,             0,              (SrcLangExt)0        }
};

//----------------------------------------------------------------------

// note that this function is not reentrant due to the use of static growBuf!
QCString escapeCharsInString(const char *name,bool allowDots,bool allowUnderscore)
{
  static bool caseSenseNames = false;
  static bool allowUnicodeNames = false;
  static GrowBuf growBuf;
  growBuf.clear();
  char c;
  const char *p=name;
  while ((c=*p++)!=0)
  {
    switch(c)
    {
      case '_': if (allowUnderscore) growBuf.addChar('_'); else growBuf.addStr("__"); break;
      case '-': growBuf.addChar('-');  break;
      case ':': growBuf.addStr("_1"); break;
      case '/': growBuf.addStr("_2"); break;
      case '<': growBuf.addStr("_3"); break;
      case '>': growBuf.addStr("_4"); break;
      case '*': growBuf.addStr("_5"); break;
      case '&': growBuf.addStr("_6"); break;
      case '|': growBuf.addStr("_7"); break;
      case '.': if (allowDots) growBuf.addChar('.'); else growBuf.addStr("_8"); break;
      case '!': growBuf.addStr("_9"); break;
      case ',': growBuf.addStr("_00"); break;
      case ' ': growBuf.addStr("_01"); break;
      case '{': growBuf.addStr("_02"); break;
      case '}': growBuf.addStr("_03"); break;
      case '?': growBuf.addStr("_04"); break;
      case '^': growBuf.addStr("_05"); break;
      case '%': growBuf.addStr("_06"); break;
      case '(': growBuf.addStr("_07"); break;
      case ')': growBuf.addStr("_08"); break;
      case '+': growBuf.addStr("_09"); break;
      case '=': growBuf.addStr("_0A"); break;
      case '$': growBuf.addStr("_0B"); break;
      case '\\': growBuf.addStr("_0C"); break;
      default: 
                if (c<0)
                {
                  char ids[5];
                  const unsigned char uc = (unsigned char)c;
                  bool doEscape = TRUE;
                  if (allowUnicodeNames && uc <= 0xf7)
                  {
                    const char* pt = p;
                    ids[ 0 ] = c;
                    int l = 0;
                    if ((uc&0xE0)==0xC0)
                    {
                      l=2; // 11xx.xxxx: >=2 byte character
                    }
                    if ((uc&0xF0)==0xE0)
                    {
                      l=3; // 111x.xxxx: >=3 byte character
                    }
                    if ((uc&0xF8)==0xF0)
                    {
                      l=4; // 1111.xxxx: >=4 byte character
                    }
                    doEscape = l==0;
                    for (int m=1; m<l && !doEscape; ++m)
                    {
                      unsigned char ct = (unsigned char)*pt;
                      if (ct==0 || (ct&0xC0)!=0x80) // invalid unicode character
                      {
                        doEscape=TRUE;
                      }
                      else
                      {
                        ids[ m ] = *pt++;
                      }
                    }
                    if ( !doEscape ) // got a valid unicode character
                    {
                      ids[ l ] = 0;
                      growBuf.addStr( ids );
                      p += l - 1;
                    }
                  }
                  if (doEscape) // not a valid unicode char or escaping needed
                  {
                    static char map[] = "0123456789ABCDEF";
                    unsigned char id = (unsigned char)c;
                    ids[0]='_';
                    ids[1]='x';
                    ids[2]=map[id>>4];
                    ids[3]=map[id&0xF];
                    ids[4]=0;
                    growBuf.addStr(ids);
                  }
                }
                else if (caseSenseNames || !isupper(c))
                {
                  growBuf.addChar(c);
                }
                else
                {
                  growBuf.addChar('_');
                  growBuf.addChar(tolower(c)); 
                }
                break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

/*! This function determines the file name on disk of an item
 *  given its name, which could be a class name with template 
 *  arguments, so special characters need to be escaped.
 */
QCString convertNameToFile(const char *name,bool allowDots,bool allowUnderscore)
{
  static bool shortNames = false;
  static bool createSubdirs = true;
  QCString result;
  if (shortNames) // use short names only
  {
    static QDict<int> usedNames(10007);
    usedNames.setAutoDelete(TRUE);
    static int count=1;

    int *value=usedNames.find(name);
    int num;
    if (value==0)
    {
      usedNames.insert(name,new int(count));
      num = count++;
    }
    else
    {
      num = *value;
    }
    result.sprintf("a%05d",num); 
  }
  else // long names
  {
    result=escapeCharsInString(name,allowDots,allowUnderscore);
    int resultLen = result.length();
    if (resultLen>=128) // prevent names that cannot be created!
    {
      // third algorithm based on MD5 hash
      uchar md5_sig[16];
      QCString sigStr(33);
      MD5Buffer((const unsigned char *)result.data(),resultLen,md5_sig);
      MD5SigToString(md5_sig,sigStr.rawData(),33);
      result=result.left(128-32)+sigStr; 
    }
  }
  if (createSubdirs)
  {
    int l1Dir=0,l2Dir=0;
    // third algorithm based on MD5 hash
    uchar md5_sig[16];
    MD5Buffer((const unsigned char *)result.data(),result.length(),md5_sig);
    l1Dir = md5_sig[14]&0xf;
    l2Dir = md5_sig[15];
    result.prepend(QCString().sprintf("d%x/d%02x/",l1Dir,l2Dir));
  }
  //printf("*** convertNameToFile(%s)->%s\n",name,result.data());
  return result;
}

/*! looks for a filter for the file \a name.  Returns the name of the filter
 *  if there is a match for the file name, otherwise an empty string.
 *  In case \a inSourceCode is TRUE then first the source filter list is
 *  considered.
 */
QCString getFileFilter(const char* name,bool isSourceCode)
{
  // sanity check
  if (name==0) return "";

  return "";
}

//----------------------------------------------------------------------------
// returns TRUE if the name of the file represented by `fi' matches
// one of the file patterns in the `patList' list.

bool patternMatch(const QFileInfo &fi,const QStrList *patList)
{
  bool found=FALSE;
  if (patList)
  { 
    QStrListIterator it(*patList);
    QCString pattern;

    QCString fn = fi.fileName().data();
    QCString fp = fi.filePath().data();
    QCString afp= fi.absFilePath().data();

    for (it.toFirst();(pattern=it.current());++it)
    {
      if (!pattern.isEmpty())
      {
        int i=pattern.find('=');
        if (i!=-1) pattern=pattern.left(i); // strip of the extension specific filter name

#if defined(_WIN32) || defined(__MACOSX__) // Windows or MacOSX
        QRegExp re(pattern,FALSE,TRUE); // case insensitive match 
#else                // unix
        QRegExp re(pattern,TRUE,TRUE);  // case sensitive match
#endif
        found = re.match(fn)!=-1 ||
                re.match(fp)!=-1 ||
                re.match(afp)!=-1;
        if (found) break;
        //printf("Matching `%s' against pattern `%s' found=%d\n",
        //    fi->fileName().data(),pattern.data(),found);
      }
    }
  }
  return found;
}

int computeQualifiedIndex(const QCString &name)
{
  int i = name.find('<');
  return name.findRev("::",i==-1 ? name.length() : i);
}

// new version by Davide Cesari which also works for Fortran
QCString stripScope(const char *name)
{
  QCString result = name;
  int l=result.length();
  int p;
  bool done = FALSE;
  bool skipBracket=FALSE; // if brackets do not match properly, ignore them altogether
  int count=0;

  do
  {
    p=l-1; // start at the end of the string
    while (p>=0 && count>=0)
    {
      char c=result.at(p);
      switch (c)
      {
        case ':': 
          // only exit in the case of ::
          //printf("stripScope(%s)=%s\n",name,result.right(l-p-1).data());
          if (p>0 && result.at(p-1)==':') return result.right(l-p-1);
          p--;
          break;
        case '>':
          if (skipBracket) // we don't care about brackets
          {
            p--;
          }
          else // count open/close brackets
          {
            if (p>0 && result.at(p-1)=='>') // skip >> operator
            {
              p-=2;
              break;
            }
            count=1;
            //printf("pos < = %d\n",p);
            p--;
            bool foundMatch=false;
            while (p>=0 && !foundMatch)
            {
              c=result.at(p--);
              switch (c)
              {
                case '>': 
                  count++; 
                  break;
                case '<': 
                  if (p>0)
                  {
                    if (result.at(p-1) == '<') // skip << operator
                    {
                      p--;
                      break;
                    }
                  }
                  count--; 
                  foundMatch = count==0;
                  break;
                default: 
                  //printf("c=%c count=%d\n",c,count);
                  break;
              }
            }
          }
          //printf("pos > = %d\n",p+1);
          break;
        default:
          p--;
      }
    }
    done = count==0 || skipBracket; // reparse if brackets do not match
    skipBracket=TRUE;
  }
  while (!done); // if < > unbalanced repeat ignoring them
  //printf("stripScope(%s)=%s\n",name,name);
  return name;
}

//----------------------------------------------------------------------

/// substitute all occurrences of \a src in \a s by \a dst
QCString substitute(const QCString &s,const QCString &src,const QCString &dst)
{
  if (s.isEmpty() || src.isEmpty()) return s;
  const char *p, *q;
  int srcLen = src.length();
  int dstLen = dst.length();
  int resLen;
  if (srcLen!=dstLen)
  {
    int count;
    for (count=0, p=s.data(); (q=strstr(p,src))!=0; p=q+srcLen) count++;
    resLen = s.length()+count*(dstLen-srcLen);
  }
  else // result has same size as s
  {
    resLen = s.length();
  }
  QCString result(resLen+1);
  char *r;
  for (r=result.rawData(), p=s; (q=strstr(p,src))!=0; p=q+srcLen)
  {
    int l = (int)(q-p);
    memcpy(r,p,l);
    r+=l;
    if (dst) memcpy(r,dst,dstLen);
    r+=dstLen;
  }
  qstrcpy(r,p);
  //printf("substitute(%s,%s,%s)->%s\n",s,src,dst,result.data());
  return result;
}

/*! Converts a string to an XML-encoded string */
QCString convertToXML(const char *s)
{
  static GrowBuf growBuf;
  growBuf.clear();
  if (s==0) return "";
  const char *p=s;
  char c;
  while ((c=*p++))
  {
    switch (c)
    {
      case '<':  growBuf.addStr("&lt;");   break;
      case '>':  growBuf.addStr("&gt;");   break;
      case '&':  growBuf.addStr("&amp;");  break;
      case '\'': growBuf.addStr("&apos;"); break; 
      case '"':  growBuf.addStr("&quot;"); break;
      case  1: case  2: case  3: case  4: case  5: case  6: case  7: case  8:
      case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
      case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26:
      case 27: case 28: case 29: case 30: case 31:
        break; // skip invalid XML characters (see http://www.w3.org/TR/2000/REC-xml-20001006#NT-Char)
      default:   growBuf.addChar(c);       break;
    }
  }
  growBuf.addChar(0);
  return growBuf.get();
}

/** Special version of QCString::stripWhiteSpace() that only strips
 *  completely blank lines.
 *  @param s the string to be stripped
 *  @param docLine the line number corresponding to the start of the
 *         string. This will be adjusted based on the number of lines stripped
 *         from the start.
 *  @returns The stripped string.
 */
QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine)
{
  const char *p = s.data();
  if (p==0) return 0;

  // search for leading empty lines
  int i=0,li=-1,l=s.length();
  char c;
  while ((c=*p++))
  {
    if (c==' ' || c=='\t' || c=='\r') i++;
    else if (c=='\n') i++,li=i,docLine++;
    else break;
  }

  // search for trailing empty lines
  int b=l-1,bi=-1;
  p=s.data()+b;
  while (b>=0)
  {
    c=*p; p--;
    if (c==' ' || c=='\t' || c=='\r') b--;
    else if (c=='\n') bi=b,b--;
    else break;
  }

  // return whole string if no leading or trailing lines where found
  if (li==-1 && bi==-1) return s;

  // return substring
  if (bi==-1) bi=l;
  if (li==-1) li=0;
  if (bi<=li) return 0; // only empty lines
  return s.mid(li,bi-li);
}

int nextUtf8CharPosition(const QCString &utf8Str,int len,int startPos)
{
  int bytes=1;
  if (startPos>=len) return len;
  char c = utf8Str[startPos];
  if (c<0) // multibyte utf-8 character
  {
    if (((uchar)c&0xE0)==0xC0)
    {
      bytes++; // 11xx.xxxx: >=2 byte character
    }
    if (((uchar)c&0xF0)==0xE0)
    {
      bytes++; // 111x.xxxx: >=3 byte character
    }
    if (((uchar)c&0xF8)==0xF0)
    {
      bytes++; // 1111.xxxx: >=4 byte character
    }
    if (((uchar)c&0xFC)==0xF8)
    {
      bytes++; // 1111.1xxx: >=5 byte character
    }
    if (((uchar)c&0xFE)==0xFC)
    {
      bytes++; // 1111.1xxx: 6 byte character
    }
  }
  else if (c=='&') // skip over character entities
  {
    static QRegExp re1("&#[0-9]+;");     // numerical entity
    static QRegExp re2("&[A-Z_a-z]+;");  // named entity
    int l1,l2;
    int i1 = re1.match(utf8Str,startPos,&l1);
    int i2 = re2.match(utf8Str,startPos,&l2);
    if (i1!=-1)
    {
      bytes=l1;
    }
    else if (i2!=-1)
    {
      bytes=l2;
    }
  }
  return startPos+bytes;
}

SrcLangExt getLanguageFromFileName(const QCString fileName)
{
  int i = fileName.findRev('.');
  if (i!=-1) // name has an extension
  {
    QCString extStr=fileName.right(fileName.length()-i).lower();
    if (!extStr.isEmpty()) // non-empty extension
    {
      int *pVal=g_extLookup.find(extStr);
      if (pVal) // listed extension
      {
        //printf("getLanguageFromFileName(%s)=%x\n",extStr.data(),*pVal);
        return (SrcLangExt)*pVal; 
      }
    }
  }
  //printf("getLanguageFromFileName(%s) not found!\n",fileName.data());
  return SrcLangExt_Cpp; // not listed => assume C-ish language.
}

QCString transcodeCharacterStringToUTF8(const QCString &input)
{
  bool error=FALSE;
  static QCString inputEncoding = "UTF-8";
  const char *outputEncoding = "UTF-8";
  if (inputEncoding.isEmpty() || qstricmp(inputEncoding,outputEncoding)==0) return input;
  int inputSize=input.length();
  int outputSize=inputSize*4+1;
  QCString output(outputSize);
  return error ? input : output;
}

/** Returns a string representation of \a lang. */
QCString langToString(SrcLangExt lang)
{
  switch(lang)
  {
    case SrcLangExt_Unknown:  return "Unknown";
    case SrcLangExt_IDL:      return "IDL";
    case SrcLangExt_Java:     return "Java";
    case SrcLangExt_CSharp:   return "C#";
    case SrcLangExt_D:        return "D";
    case SrcLangExt_PHP:      return "PHP";
    case SrcLangExt_ObjC:     return "Objective-C";
    case SrcLangExt_Cpp:      return "C++";
    case SrcLangExt_JS:       return "Javascript";
    case SrcLangExt_Python:   return "Python";
    case SrcLangExt_Fortran:  return "Fortran";
    case SrcLangExt_VHDL:     return "VHDL";
    case SrcLangExt_XML:      return "XML";
    case SrcLangExt_Tcl:      return "Tcl";
    case SrcLangExt_Markdown: return "Markdown";
  }
  return "Unknown";
}

/** Returns the scope separator to use given the programming language \a lang */
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope)
{
  if (lang==SrcLangExt_Java || lang==SrcLangExt_CSharp || lang==SrcLangExt_VHDL || lang==SrcLangExt_Python)
  {
    return ".";
  }
  else if (lang==SrcLangExt_PHP && !classScope)
  {
    return "\\";
  }
  else
  {
    return "::";
  }
}
