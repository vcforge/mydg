#include "StdAfx.h"
#include "qtools/qdatastream.h"
#include "qtools/qdict.h"
#include "qtools/qlist.h"
#include "qtools/qcstring.h"
#include "qtools/qdir.h"
#include "doxygen.h"
#include "filename.h"

FileNameDict    *includeNameDict = new FileNameDict(10007);     // include names
static StringList       g_inputFiles;

int readFileOrDirectory(const char *s,
                        FileNameList *fnList,
                        FileNameDict *fnDict,
                        StringDict *exclDict,
                        QStrList *patList,
                        QStrList *exclPatList,
                        StringList *resultList,
                        StringDict *resultDict,
                        bool recursive,
                        bool errorIfNotExist=TRUE,
                        QDict<void> *killDict = 0,
                        QDict<void> *paths = 0
                       );

//! parse the list of input files
static void parseFiles()
{
  {
    StringListIterator it(g_inputFiles);
    QCString *s;
    for (;(s=it.current());++it)
    {
    }
  }
}

//----------------------------------------------------------------------------
// read a file or all files in a directory and append their contents to the
// input string. The names of the files are appended to the `fiList' list.

int readFileOrDirectory(const char *s,
                        FileNameList *fnList,
                        FileNameDict *fnDict,
                        StringDict *exclDict,
                        QStrList *patList,
                        QStrList *exclPatList,
                        StringList *resultList,
                        StringDict *resultDict,
                        bool recursive,
                        bool errorIfNotExist,
                        QDict<void> *killDict,
                        QDict<void> *paths
                       )
{
  //printf("killDict=%p count=%d\n",killDict,killDict->count());
  // strip trailing slashes
  if (s==0) return 0;
  QCString fs = s;
  char lc = fs.at(fs.length()-1);
  if (lc=='/' || lc=='\\') fs = fs.left(fs.length()-1);

  QFileInfo fi(fs);
  int totalSize=0;
  return totalSize;
}

void searchInputFiles()
{
  QStrList exclPatterns;
  bool alwaysRecursive = true;
  StringDict excludeNameDict(1009);
  excludeNameDict.setAutoDelete(TRUE);

  // gather names of all files in the include path
  QStrList includePathList;
  includePathList.append("C:\\Projects\\proj");
  char *s=includePathList.first();
  while (s)
  {
    QStrList pl;
	pl.append("*.c");
    readFileOrDirectory(s,0,includeNameDict,0,&pl,
                        &exclPatterns,0,0,
                        alwaysRecursive);
    s=includePathList.next(); 
  }
}

void parseInput()
{
//  atexit(exitDoxygen);
  /**************************************************************************
   *            Make sure the output directory exists
   **************************************************************************/
  QString string;
  QCString outputDirectory = "C:\\Projects\\docs";
  if (outputDirectory.isEmpty()) 
  {
    outputDirectory=QDir::currentDirPath().utf8();
  }
  else
  {
    QDir dir(outputDirectory);
    if (!dir.exists())
    {
      dir.setPath(QDir::currentDirPath());
      if (!dir.mkdir(outputDirectory))
      {
//        cleanUpDoxygen();
        exit(1);
      }
      else
      {
      }
      dir.cd(outputDirectory);
    }
    outputDirectory=dir.absPath().utf8();
  }

  searchInputFiles();

  parseFiles();
}
