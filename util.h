#include "qtools/qcstring.h"
#include "qtools/qfileinfo.h"
#include "types.h"

inline bool isId(int c)
{
  return c=='_' || c>=128 || c<0 || isalnum(c);
}

QCString convertNameToFile(const char *name,bool allowDots=FALSE,bool allowUnderscore=FALSE);

QCString langToString(SrcLangExt lang);
QCString getLanguageSpecificSeparator(SrcLangExt lang,bool classScope=FALSE);

QCString getFileFilter(const char* name,bool isSourceCode);

bool patternMatch(const QFileInfo &fi,const QStrList *patList);

int computeQualifiedIndex(const QCString &name);

QCString transcodeCharacterStringToUTF8(const QCString &input);

SrcLangExt getLanguageFromFileName(const QCString fileName);

QCString stripScope(const char *name);

QCString convertToXML(const char *s);

QCString escapeCharsInString(const char *name,bool allowDots,bool allowUnderscore=FALSE);

QCString substitute(const QCString &s,const QCString &src,const QCString &dst);

QCString stripLeadingAndTrailingEmptyLines(const QCString &s,int &docLine);

int nextUtf8CharPosition(const QCString &utf8Str,int len,int startPos);
