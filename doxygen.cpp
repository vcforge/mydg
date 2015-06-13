#include "StdAfx.h"
#include "qtools/qdatastream.h"
#include "qtools/qdict.h"
#include "qtools/qlist.h"
#include "qtools/qcstring.h"
#include "qtools/qdir.h"
#include "util.h"
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
// Read all files matching at least one pattern in `patList' in the 
// directory represented by `fi'.
// The directory is read iff the recusiveFlag is set.
// The contents of all files is append to the input string

int readDir(QFileInfo *fi,
            FileNameList *fnList,
            FileNameDict *fnDict,
            StringDict  *exclDict,
            QStrList *patList,
            QStrList *exclPatList,
            StringList *resultList,
            StringDict *resultDict,
            bool errorIfNotExist,
            bool recursive,
            QDict<void> *killDict,
            QDict<void> *paths
           )
{
  QCString dirName = fi->absFilePath().utf8();
  if (paths && paths->find(dirName)==0)
  {
    paths->insert(dirName,(void*)0x8);
  }
  if (fi->isSymLink())
  {
  }
  QDir dir(dirName);
  dir.setFilter( QDir::Files | QDir::Dirs | QDir::Hidden );
  int totalSize=0;
  //printf("killDict=%p count=%d\n",killDict,killDict->count());
  
  const QFileInfoList *list = dir.entryInfoList();
  if (list)
  {
    QFileInfoListIterator it( *list );
    QFileInfo *cfi;

    while ((cfi=it.current()))
    {
      if (exclDict==0 || exclDict->find(cfi->absFilePath().utf8())==0) 
      { // file should not be excluded
        //printf("killDict->find(%s)\n",cfi->absFilePath().data());
        if (!cfi->exists() || !cfi->isReadable())
        {
          if (errorIfNotExist)
          {
          }
        }
        else if (cfi->isFile() && 
            (patList==0 || patternMatch(*cfi,patList)) && 
            !patternMatch(*cfi,exclPatList) &&
            (killDict==0 || killDict->find(cfi->absFilePath().utf8())==0)
            )
        {
          totalSize+=cfi->size()+cfi->absFilePath().length()+4;
          QCString name=cfi->fileName().utf8();
          //printf("New file %s\n",name.data());
          if (fnDict)
          {
            FileDef  *fd=new FileDef(cfi->dirPath().utf8()+"/",name);
            FileName *fn=0;
            if (!name.isEmpty() && (fn=(*fnDict)[name]))
            {
              fn->append(fd);
            }
            else
            {
              fn = new FileName(cfi->absFilePath().utf8(),name);
              fn->append(fd);
              if (fnList) fnList->inSort(fn);
              fnDict->insert(name,fn);
            }
          }
          QCString *rs=0;
          if (resultList || resultDict)
          {
            rs=new QCString(cfi->absFilePath().utf8());
          }
          if (resultList) resultList->append(rs);
          if (resultDict) resultDict->insert(cfi->absFilePath().utf8(),rs);
          if (killDict) killDict->insert(cfi->absFilePath().utf8(),(void *)0x8);
        }
        else if (recursive &&
            cfi->isDir() && 
            !patternMatch(*cfi,exclPatList) &&
            cfi->fileName().at(0)!='.') // skip "." ".." and ".dir"
        {
          cfi->setFile(cfi->absFilePath());
          totalSize+=readDir(cfi,fnList,fnDict,exclDict,
              patList,exclPatList,resultList,resultDict,errorIfNotExist,
              recursive,killDict,paths);
        }
      }
      ++it;
    }
  }
  return totalSize;
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
  {
    if (exclDict==0 || exclDict->find(fi.absFilePath().utf8())==0)
    {
      if (!fi.exists() || !fi.isReadable())
      {
        if (errorIfNotExist)
        {
        }
	  }
	  else
	  {
        if (fi.isFile())
        {
          QCString dirPath = fi.dirPath(TRUE).utf8();
          QCString filePath = fi.absFilePath().utf8();
          if (paths && paths->find(dirPath))
          {
            paths->insert(dirPath,(void*)0x8);
          }
          //printf("killDict->find(%s)\n",fi.absFilePath().data());
          if (killDict==0 || killDict->find(filePath)==0)
          {
            totalSize+=fi.size()+fi.absFilePath().length()+4; //readFile(&fi,fiList,input); 
            //fiList->inSort(new FileInfo(fi));
            QCString name=fi.fileName().utf8();
            //printf("New file %s\n",name.data());
            if (fnDict)
            {
              FileDef  *fd=new FileDef(dirPath+"/",name);
              FileName *fn=0;
              if (!name.isEmpty() && (fn=(*fnDict)[name]))
              {
                fn->append(fd);
              }
              else
              {
                fn = new FileName(filePath,name);
                fn->append(fd);
                if (fnList) fnList->inSort(fn);
                fnDict->insert(name,fn);
              }
            }
            QCString *rs=0;
            if (resultList || resultDict)
            {
              rs=new QCString(filePath);
              if (resultList) resultList->append(rs);
              if (resultDict) resultDict->insert(filePath,rs);
            }

            if (killDict) killDict->insert(fi.absFilePath().utf8(),(void *)0x8);
          }
        }
        else if (fi.isDir()) // readable dir
        {
          totalSize+=readDir(&fi,fnList,fnDict,exclDict,patList,
              exclPatList,resultList,resultDict,errorIfNotExist,
              recursive,killDict,paths);
        }
	  }
	}
  }
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
	pl.append("*.h");
	pl.append("*.cpp");
    readFileOrDirectory(s,0,includeNameDict,0,&pl,
                        &exclPatterns,0,0,
                        alwaysRecursive);
    s=includePathList.next(); 
  }

  /**************************************************************************
   *             Determine Input Files                                      *
   **************************************************************************/

  QDict<void> *killDict = new QDict<void>(10007);
  QStrList inputList;
  inputList.append("C:\\Projects\\proj");
  g_inputFiles.setAutoDelete(TRUE);
  s=inputList.first();
  QDict<void>      inputPaths(1009);
  while (s)
  {
    QCString path=s;
    uint l = path.length();
    if (l>0)
    {
      // strip trailing slashes
      if (path.at(l-1)=='\\' || path.at(l-1)=='/') path=path.left(l-1);

      QStrList pl;
	  pl.append("*.h");
	  pl.append("*.cpp");
      readFileOrDirectory(
          path,
          0,
          0,
          &excludeNameDict,
          &pl,
          &exclPatterns,
          &g_inputFiles,0,
          alwaysRecursive,
          TRUE,
          killDict,
          &inputPaths);
    }
    s=inputList.next();
  }
  delete killDict;
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
