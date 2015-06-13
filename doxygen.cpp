#include "StdAfx.h"
#include "qtools/qdatastream.h"
#include "qtools/qdict.h"
#include "qtools/qlist.h"
#include "qtools/qcstring.h"
#include "qtools/qdir.h"
#include "qtools/qcache.h"
#include "util.h"
#include "doxygen.h"
#include "portable.h"
#include "filename.h"
#include "filedef.h"
#include "parserintf.h"
#include "bufstr.h"
#include "commentcnv.h"
#include "outputgen.h"
#include "fileparser.h"

ParserManager   *parserManager   = NULL;
FileNameList    *inputNameList   = new FileNameList;       // all input files
FileNameDict    *inputNameDict   = new FileNameDict(10007);
FileNameDict    *includeNameDict = new FileNameDict(10007);     // include names
static StringList       g_inputFiles;

//----------------------------------------------------------------------

/** Cache element for the file name to FileDef mapping cache. */
struct FindFileCacheElem
{
  FindFileCacheElem(FileDef *fd,bool ambig) : fileDef(fd), isAmbig(ambig) {}
  FileDef *fileDef;
  bool isAmbig;
};

static QCache<FindFileCacheElem> g_findFileDefCache(5000);

FileDef *findFileDef(const FileNameDict *fnDict,const char *n,bool &ambig)
{
  ambig=FALSE;
  if (n==0) return 0;

  const int maxAddrSize = 20;
  char addr[maxAddrSize];
  qsnprintf(addr,maxAddrSize,"%p:",fnDict);
  QCString key = addr;
  key+=n;

  g_findFileDefCache.setAutoDelete(TRUE);
  FindFileCacheElem *cachedResult = g_findFileDefCache.find(key);
  //printf("key=%s cachedResult=%p\n",key.data(),cachedResult);
  if (cachedResult)
  {
    ambig = cachedResult->isAmbig;
    //printf("cached: fileDef=%p\n",cachedResult->fileDef);
    return cachedResult->fileDef;
  }
  else
  {
    cachedResult = new FindFileCacheElem(0,FALSE);
  }

  QCString name=QDir::cleanDirPath(n).utf8();
  QCString path;
  int slashPos;
  FileName *fn;
  if (name.isEmpty()) goto exit;
  slashPos=QMAX(name.findRev('/'),name.findRev('\\'));
  if (slashPos!=-1)
  {
    path=name.left(slashPos+1);
    name=name.right(name.length()-slashPos-1); 
    //printf("path=%s name=%s\n",path.data(),name.data());
  }
  if (name.isEmpty()) goto exit;
  if ((fn=(*fnDict)[name]))
  {
    //printf("fn->count()=%d\n",fn->count());
    if (fn->count()==1)
    {
      FileDef *fd = fn->getFirst();
#if defined(_WIN32) || defined(__MACOSX__) // Windows or MacOSX
      bool isSamePath = fd->getPath().right(path.length()).lower()==path.lower();
#else // Unix
      bool isSamePath = fd->getPath().right(path.length())==path;
#endif
      if (path.isEmpty() || isSamePath)
      {
        cachedResult->fileDef = fd;
        g_findFileDefCache.insert(key,cachedResult);
        //printf("=1 ===> add to cache %p\n",fd);
        return fd;
      }
    }
    else // file name alone is ambiguous
    {
      int count=0;
      FileNameIterator fni(*fn);
      FileDef *fd;
      FileDef *lastMatch=0;
      QCString pathStripped = path;
      for (fni.toFirst();(fd=fni.current());++fni)
      {
        QCString fdStripPath = fd->getPath();
        if (path.isEmpty() || fdStripPath.right(pathStripped.length())==pathStripped) 
        { 
          count++; 
          lastMatch=fd; 
        }
      }
      //printf(">1 ===> add to cache %p\n",fd);

      ambig=(count>1);
      cachedResult->isAmbig = ambig;
      cachedResult->fileDef = lastMatch;
      g_findFileDefCache.insert(key,cachedResult);
      return lastMatch;
    }
  }
  else
  {
    //printf("not found!\n");
  }
exit:
  //printf("0  ===> add to cache %p: %s\n",cachedResult,n);
  g_findFileDefCache.insert(key,cachedResult);
  //delete cachedResult;
  return 0;
}

//----------------------------------------------------------------------------

void initDoxygen()
{
  parserManager = new ParserManager;
  parserManager->registerDefaultParser(         new FileParser);
}

void cleanUpDoxygen()
{
  delete parserManager;
}

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

//----------------------------------------------------------------------------

/*! takes the \a buf of the given length \a len and converts CR LF (DOS)
 * or CR (MAC) line ending to LF (Unix).  Returns the length of the
 * converted content (i.e. the same as \a len (Unix, MAC) or
 * smaller (DOS).
 */
int filterCRLF(char *buf,int len)
{
  int src = 0;    // source index
  int dest = 0;   // destination index
  char c;         // current character

  while (src<len)
  {
    c = buf[src++];            // Remember the processed character.
    if (c == '\r')             // CR to be solved (MAC, DOS)
    {
      c = '\n';                // each CR to LF
      if (src<len && buf[src] == '\n')
        ++src;                 // skip LF just after CR (DOS) 
    }
    else if ( c == '\0' && src<len-1) // filter out internal \0 characters, as it will confuse the parser
    {
      c = ' ';                 // turn into a space
    }
    buf[dest++] = c;           // copy the (modified) character to dest
  }
  return dest;                 // length of the valid part of the buf
}

//! read a file name \a fileName and optionally filter and transcode it
bool readInputFile(const char *fileName,BufStr &inBuf,bool filter=TRUE,bool isSourceCode=FALSE)
{
  // try to open file
  int size=0;
  //uint oldPos = dest.curPos();
  //printf(".......oldPos=%d\n",oldPos);

  QFileInfo fi(fileName);
  if (!fi.exists()) return FALSE;
  QCString filterName = getFileFilter(fileName,isSourceCode);
  if (filterName.isEmpty() || !filter)
  {
    QFile f(fileName);
    if (!f.open(IO_ReadOnly))
    {
      return FALSE;
    }
    size=fi.size();
    // read the file
    inBuf.skip(size);
    if (f.readBlock(inBuf.data()/*+oldPos*/,size)!=size)
    {
      return FALSE;
    }
  }
  else
  {
    QCString cmd=filterName+" \""+fileName+"\"";
    FILE *f=portable_popen(cmd,"r");
    if (!f)
    {
      return FALSE;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead;
    while ((numRead=(int)fread(buf,1,bufSize,f))>0)
    {
      //printf(">>>>>>>>Reading %d bytes\n",numRead);
      inBuf.addArray(buf,numRead),size+=numRead;
    }
    portable_pclose(f);
    inBuf.at(inBuf.curPos()) ='\0';
  }

  int start=0;

  //inBuf.addChar('\n'); /* to prevent problems under Windows ? */

  // and translate CR's
  size=inBuf.curPos()-start;
  int newSize=filterCRLF(inBuf.data()+start,size);
  //printf("filter char at %p size=%d newSize=%d\n",dest.data()+oldPos,size,newSize);
  if (newSize!=size) // we removed chars
  {
    inBuf.shrink(newSize); // resize the array
    //printf(".......resizing from %d to %d result=[%s]\n",oldPos+size,oldPos+newSize,dest.data());
  }
  inBuf.addChar(0);
  return TRUE;
}

//----------------------------------------------------------------------------

static ParserInterface *getParserForFile(const char *fn)
{
  QCString fileName=fn;
  QCString extension;
  int sep = fileName.findRev('/');
  int ei = fileName.findRev('.');
  if (ei!=-1 && (sep==-1 || ei>sep)) // matches dir/file.ext but not dir.1/file
  {
    extension=fileName.right(fileName.length()-ei);
  }
  else
  {
    extension = ".no_extension";
  }

  return parserManager->getParser(extension);
}

static void parseFile(ParserInterface *parser,
                      FileDef *fd,const char *fn,
                      bool sameTu,QStrList &filesInSameTu)
{
  static bool clangAssistedParsing = FALSE;
  QCString fileName=fn;
  QCString extension;
  int ei = fileName.findRev('.');
  if (ei!=-1)
  {
    extension=fileName.right(fileName.length()-ei);
  }
  else
  {
    extension = ".no_extension";
  }

  QFileInfo fi(fileName);
  BufStr preBuf(fi.size()+4096);

  if (true && 
      parser->needsPreprocessing(extension))
  {
    BufStr inBuf(fi.size()+4096);
    readInputFile(fileName,inBuf);
//    preprocessFile(fileName,inBuf,preBuf);
  }
  else // no preprocessing
  {
    readInputFile(fileName,preBuf);
  }
  if (preBuf.data() && preBuf.curPos()>0 && *(preBuf.data()+preBuf.curPos()-1)!='\n')
  {
    preBuf.addChar('\n'); // add extra newline to help parser
  }

  BufStr convBuf(preBuf.curPos()+1024);

  // convert multi-line C++ comments to C style comments
  convertCppComments(&preBuf,&convBuf,fileName);

  convBuf.addChar('\0');
}

//! parse the list of input files
static void parseFiles()
{
  {
    StringListIterator it(g_inputFiles);
    QCString *s;
    for (;(s=it.current());++it)
    {
      bool ambig;
      QStrList filesInSameTu;
      FileDef *fd=findFileDef(inputNameDict,s->data(),ambig);
      ASSERT(fd!=0);
      ParserInterface * parser = getParserForFile(s->data());
	  if( parser )
	  {
        parser->startTranslationUnit(s->data());
        parseFile(parser,fd,s->data(),FALSE,filesInSameTu);
	  }
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
          inputNameList,
          inputNameDict,
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
