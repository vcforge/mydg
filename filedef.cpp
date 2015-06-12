/******************************************************************************
 *
 * 
 *
 * Copyright (C) 1997-2014 by Dimitri van Heesch.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation under the terms of the GNU General Public License is hereby 
 * granted. No representations are made about the suitability of this software 
 * for any purpose. It is provided "as is" without express or implied warranty.
 * See the GNU General Public License for more details.
 *
 * Documents produced by Doxygen are derivative works derived from the
 * input used in their production; they are not affected by this license.
 *
 */

#include "StdAfx.h"
#include "qtools/qdatastream.h"
#include "qtools/qdict.h"
#include "qtools/qlist.h"
#include "qtools/qcstring.h"
#include "qtools/qdir.h"
#include "filedef.h"
#include "util.h"
#include "debug.h"
#include "message.h"
#include "config.h"
#include "portable.h"

//---------------------------------------------------------------------------

/*! create a new file definition, where \a p is the file path, 
    \a nm the file name, and \a lref is an HTML anchor name if the
    file was read from a tag file or 0 otherwise
*/
FileDef::FileDef(const char *p,const char *nm,
                 const char *lref,const char *dn)
   : Definition((QCString)p+nm,1,1,nm)
{
  m_path=p;
  m_filePath=m_path+nm;
  m_fileName=nm;
  m_diskName=dn;
  if (m_diskName.isEmpty()) m_diskName=nm;
  setReference(lref);
  m_includeList       = 0;
  m_includeDict       = 0; 
  m_includedByList    = 0;
  m_includedByDict    = 0; 
  m_srcDefDict        = 0;
  m_usingDeclList     = 0;
  m_isSource          = 0; 
  m_docname           = nm;
  setLanguage(getLanguageFromFileName(name()));
  acquireFileVersion();
  m_subGrouping=true;
}

/*! destroy the file definition */
FileDef::~FileDef()
{
  delete m_includeDict;
  delete m_includeList;
  delete m_includedByDict;
  delete m_includedByList;
  delete m_srcDefDict;
  delete m_usingDeclList;
}

/*! Compute the HTML anchor names for all members in the class */ 
void FileDef::computeAnchors()
{
}

void FileDef::distributeMemberGroupDocumentation()
{
  //printf("FileDef::distributeMemberGroupDocumentation()\n");
}

void FileDef::findSectionsInDocumentation()
{
}

bool FileDef::hasDetailedDescription() const
{
  static bool repeatBrief = true;
  static bool sourceBrowser = false;
  return ((!briefDescription().isEmpty() && repeatBrief) || 
          !documentation().stripWhiteSpace().isEmpty() || // avail empty section
          (sourceBrowser && getStartBodyLine()!=-1 && getBodyDef())
         );
}

void FileDef::writeTagFile(FTextStream &tagFile)
{
  tagFile << "  <compound kind=\"file\">" << endl;
  tagFile << "    <name>" << convertToXML(name()) << "</name>" << endl;
  tagFile << "    <path>" << convertToXML(getPath()) << "</path>" << endl;
  tagFile << "    <filename>" << convertToXML(getOutputFileBase()) << "</filename>" << endl;
  if (m_includeList && m_includeList->count()>0)
  {
    QListIterator<IncludeInfo> ili(*m_includeList);
    IncludeInfo *ii;
    for (;(ii=ili.current());++ili)
    {
      if (!ii->indirect)
      {
        FileDef *fd=ii->fileDef;
        if (fd && fd->isLinkable() && !fd->isReference()) 
        {
          bool isIDLorJava = FALSE;
          SrcLangExt lang = fd->getLanguage();
          isIDLorJava = lang==SrcLangExt_IDL || lang==SrcLangExt_Java;
          const char *locStr = (ii->local    || isIDLorJava) ? "yes" : "no";
          const char *impStr = (ii->imported || isIDLorJava) ? "yes" : "no";
          tagFile << "    <includes id=\"" 
                  << convertToXML(fd->getOutputFileBase()) << "\" "
                  << "name=\"" << convertToXML(fd->name()) << "\" "
                  << "local=\"" << locStr << "\" "
                  << "imported=\"" << impStr << "\">"
                  << convertToXML(ii->includeName)
                  << "</includes>" 
                  << endl;
        }
      }
    }
  }

  writeDocAnchorsToTagFile(tagFile);
  tagFile << "  </compound>" << endl;
}

void FileDef::parseSource(bool sameTu,QStrList &filesInSameTu)
{
  static bool filterSourceFiles = false;
  {
  }
}

void FileDef::startParsing()
{
}

void FileDef::finishParsing()
{
}

void FileDef::addMembersToMemberGroup()
{
}

QCString FileDef::name() const 
{ 
  if (Config_getBool("FULL_PATH_NAMES")) 
    return m_fileName; 
  else 
    return Definition::name(); 
} 

void FileDef::addSourceRef(int line,Definition *d,MemberDef *md)
{
  //printf("FileDef::addSourceDef(%d,%p,%p)\n",line,d,md);
  if (d)
  {
    if (m_srcDefDict==0)    m_srcDefDict    = new QIntDict<Definition>(257);
    m_srcDefDict->insert(line,d);
    //printf("Adding member %s with anchor %s at line %d to file %s\n",
    //    md?md->name().data():"<none>",md?md->anchor().data():"<none>",line,name().data());
  }
}

Definition *FileDef::getSourceDefinition(int lineNr) const
{
  Definition *result=0;
  if (m_srcDefDict)
  {
    result = m_srcDefDict->find(lineNr);
  }
  //printf("%s::getSourceDefinition(%d)=%s\n",name().data(),lineNr,result?result->name().data():"none");
  return result;
}

void FileDef::addUsingDeclaration(Definition *d)
{
  if (m_usingDeclList==0)
  {
    m_usingDeclList = new SDict<Definition>(17);
  }
  if (m_usingDeclList->find(d->qualifiedName())==0)
  {
    m_usingDeclList->append(d->qualifiedName(),d);
  }
}

void FileDef::addIncludeDependency(FileDef *fd,const char *incName,bool local,
                                   bool imported,bool indirect)
{
  //printf("FileDef::addIncludeDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && (!m_includeDict || m_includeDict->find(iName)==0))
  {
    if (m_includeDict==0)
    {
      m_includeDict   = new QDict<IncludeInfo>(61);
      m_includeList   = new QList<IncludeInfo>;
      m_includeList->setAutoDelete(TRUE);
    }
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    ii->indirect    = indirect;
    m_includeList->append(ii);
    m_includeDict->insert(iName,ii);
  }
}

void FileDef::addIncludedUsingDirectives()
{
  if (visited) return;
  visited=TRUE;
  //printf("( FileDef::addIncludedUsingDirectives for file %s\n",name().data());

  if (m_includeList) // file contains #includes
  {
    {
      QListIterator<IncludeInfo> iii(*m_includeList);
      IncludeInfo *ii;
      for (iii.toFirst();(ii=iii.current());++iii) // foreach #include...
      {
        if (ii->fileDef && !ii->fileDef->visited) // ...that is a known file
        {
          // recurse into this file
          ii->fileDef->addIncludedUsingDirectives();
        }
      }
    }
    {
      QListIterator<IncludeInfo> iii(*m_includeList);
      IncludeInfo *ii;
      // iterate through list from last to first
      for (iii.toLast();(ii=iii.current());--iii)
      {
        if (ii->fileDef && ii->fileDef!=this)
        {
          // add using declarations
          SDict<Definition> *udl = ii->fileDef->m_usingDeclList;
          if (udl)
          {
            SDict<Definition>::Iterator udi(*udl);
            Definition *d;
            for (udi.toLast();(d=udi.current());--udi)
            {
              //printf("Adding using declaration %s\n",d->name().data());
              if (m_usingDeclList==0)
              {
                m_usingDeclList = new SDict<Definition>(17);
              }
              if (m_usingDeclList->find(d->qualifiedName())==0)
              {
                m_usingDeclList->prepend(d->qualifiedName(),d);
              }
            }
          }
        }
      }
    }
  }
  //printf(") end FileDef::addIncludedUsingDirectives for file %s\n",name().data());
}


void FileDef::addIncludedByDependency(FileDef *fd,const char *incName,
                                      bool local,bool imported)
{
  //printf("FileDef::addIncludedByDependency(%p,%s,%d)\n",fd,incName,local);
  QCString iName = fd ? fd->absFilePath().data() : incName;
  if (!iName.isEmpty() && (m_includedByDict==0 || m_includedByDict->find(iName)==0))
  {
    if (m_includedByDict==0)
    {
      m_includedByDict = new QDict<IncludeInfo>(61);
      m_includedByList = new QList<IncludeInfo>;
      m_includedByList->setAutoDelete(TRUE);
    }
    IncludeInfo *ii = new IncludeInfo;
    ii->fileDef     = fd;
    ii->includeName = incName;
    ii->local       = local;
    ii->imported    = imported;
    ii->indirect    = FALSE;
    m_includedByList->append(ii);
    m_includedByDict->insert(iName,ii);
  }
}

bool FileDef::isIncluded(const QCString &name) const
{
  if (name.isEmpty()) return FALSE;
  return m_includeDict!=0 && m_includeDict->find(name)!=0;
}

bool FileDef::generateSourceFile() const 
{ 
  static bool sourceBrowser = Config_getBool("SOURCE_BROWSER");
  static bool verbatimHeaders = Config_getBool("VERBATIM_HEADERS");
  QCString extension = name().right(4);
  return !isReference() && 
         (sourceBrowser || 
           (verbatimHeaders) 
         ) &&
         extension!=".doc" && extension!=".txt" && extension!=".dox" &&
         extension!=".md" && name().right(9)!=".markdown";  
}


void FileDef::addListReferences()
{
  {
  }
}

//-------------------------------------------------------------------

static int findMatchingPart(const QCString &path,const QCString dir)
{
  int si1;
  int pos1=0,pos2=0;
  while ((si1=path.find('/',pos1))!=-1)
  {
    int si2=dir.find('/',pos2);
    //printf("  found slash at pos %d in path %d: %s<->%s\n",si1,si2,
    //    path.mid(pos1,si1-pos2).data(),dir.mid(pos2).data());
    if (si2==-1 && path.mid(pos1,si1-pos2)==dir.mid(pos2)) // match at end
    {
      return dir.length();
    }
    if (si1!=si2 || path.mid(pos1,si1-pos2)!=dir.mid(pos2,si2-pos2)) // no match in middle
    {
      return QMAX(pos1-1,0);
    }
    pos1=si1+1;
    pos2=si2+1;
  }
  return 0;
}

static Directory *findDirNode(Directory *root,const QCString &name)
{
  QListIterator<DirEntry> dli(root->children());
  DirEntry *de;
  for (dli.toFirst();(de=dli.current());++dli)
  {
    if (de->kind()==DirEntry::Dir)
    {
      Directory *dir = (Directory *)de;
      QCString dirName=dir->name();
      int sp=findMatchingPart(name,dirName);
      //printf("findMatchingPart(%s,%s)=%d\n",name.data(),dirName.data(),sp);
      if (sp>0) // match found
      {
        if ((uint)sp==dirName.length()) // whole directory matches
        {
          // recurse into the directory
          return findDirNode(dir,name.mid(dirName.length()+1));
        } 
        else // partial match => we need to split the path into three parts
        {
          QCString baseName     =dirName.left(sp);
          QCString oldBranchName=dirName.mid(sp+1);
          QCString newBranchName=name.mid(sp+1);
          // strip file name from path
          int newIndex=newBranchName.findRev('/');
          if (newIndex>0) newBranchName=newBranchName.left(newIndex);

          //printf("Splitting off part in new branch \n"
          //    "base=%s old=%s new=%s\n",
          //    baseName.data(),
          //    oldBranchName.data(),
          //    newBranchName.data()
          //      );
          Directory *base = new Directory(root,baseName);
          Directory *newBranch = new Directory(base,newBranchName);
          dir->reParent(base);
          dir->rename(oldBranchName);
          base->addChild(dir);
          base->addChild(newBranch);
          dir->setLast(FALSE);
          // remove DirEntry container from list (without deleting it)
          root->children().setAutoDelete(FALSE);
          root->children().removeRef(dir);
          root->children().setAutoDelete(TRUE);
          // add new branch to the root
          if (!root->children().isEmpty())
          {
            root->children().getLast()->setLast(FALSE); 
          }
          root->addChild(base);
          return newBranch;
        }
      }
    }
  }
  int si=name.findRev('/');
  if (si==-1) // no subdir
  {
    return root; // put the file under the root node.
  }
  else // need to create a subdir 
  {
    QCString baseName = name.left(si);
    //printf("new subdir %s\n",baseName.data());
    Directory *newBranch = new Directory(root,baseName);
    if (!root->children().isEmpty())
    {
      root->children().getLast()->setLast(FALSE); 
    }
    root->addChild(newBranch);
    return newBranch;
  }
}

static void mergeFileDef(Directory *root,FileDef *fd)
{
  QCString filePath = fd->absFilePath();
  //printf("merging %s\n",filePath.data());
  Directory *dirNode = findDirNode(root,filePath);
  if (!dirNode->children().isEmpty())
  {
    dirNode->children().getLast()->setLast(FALSE); 
  }
  DirEntry *e=new DirEntry(dirNode,fd);
  dirNode->addChild(e);
}

#if 0
static void generateIndent(QTextStream &t,DirEntry *de,int level)
{
  if (de->parent())
  {
    generateIndent(t,de->parent(),level+1);
  }
  // from the root up to node n do...
  if (level==0) // item before a dir or document
  {
    if (de->isLast())
    {
      if (de->kind()==DirEntry::Dir)
      {
        t << "<img " << FTV_IMGATTRIBS(plastnode) << "/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(lastnode) << "/>";
      }
    }
    else
    {
      if (de->kind()==DirEntry::Dir)
      {
        t << "<img " << FTV_IMGATTRIBS(pnode) << "/>";
      }
      else
      {
        t << "<img " << FTV_IMGATTRIBS(node) << "/>";
      }
    }
  }
  else // item at another level
  {
    if (de->isLast())
    {
      t << "<img " << FTV_IMGATTRIBS(blank) << "/>";
    }
    else
    {
      t << "<img " << FTV_IMGATTRIBS(vertline) << "/>";
    }
  }
}

static void writeDirTreeNode(QTextStream &t,Directory *root,int level)
{
  QCString indent;
  indent.fill(' ',level*2);
  QListIterator<DirEntry> dli(root->children());
  DirEntry *de;
  for (dli.toFirst();(de=dli.current());++dli)
  {
    t << indent << "<p>";
    generateIndent(t,de,0);
    if (de->kind()==DirEntry::Dir)
    {
      Directory *dir=(Directory *)de;
      //printf("%s [dir]: %s (last=%d,dir=%d)\n",indent.data(),dir->name().data(),dir->isLast(),dir->kind()==DirEntry::Dir);
      t << "<img " << FTV_IMGATTRIBS(folderclosed) << "/>";
      t << dir->name();
      t << "</p>\n";
      t << indent << "<div>\n";
      writeDirTreeNode(t,dir,level+1);
      t << indent << "</div>\n";
    }
    else
    {
      //printf("%s [file]: %s (last=%d,dir=%d)\n",indent.data(),de->file()->name().data(),de->isLast(),de->kind()==DirEntry::Dir);
      t << "<img " << FTV_IMGATTRIBS(doc) << "/>";
      t << de->file()->name();
      t << "</p>\n";
    }
  }
}
#endif

static void addDirsAsGroups(Directory *root,GroupDef *parent,int level)
{
}

void generateFileTree()
{
}

//-------------------------------------------------------------------

void FileDef::combineUsingRelations()
{
}

bool FileDef::isDocumentationFile() const
{
  return name().right(4)==".doc" ||
         name().right(4)==".txt" ||
         name().right(4)==".dox" ||
         name().right(3)==".md"  ||
         name().right(9)==".markdown";
}

void FileDef::acquireFileVersion()
{
  QCString vercmd = Config_getString("FILE_VERSION_FILTER");
  if (!vercmd.isEmpty() && !m_filePath.isEmpty() && m_filePath!="generated") 
  {
    msg("Version of %s : ",m_filePath.data());
    QCString cmd = vercmd+" \""+m_filePath+"\"";
    Debug::print(Debug::ExtCmd,0,"Executing popen(`%s`)\n",cmd.data());
    FILE *f=portable_popen(cmd,"r");
    if (!f)
    {
      err("could not execute %s\n",vercmd.data());
      return;
    }
    const int bufSize=1024;
    char buf[bufSize];
    int numRead = (int)fread(buf,1,bufSize-1,f);
    portable_pclose(f);
    if (numRead>0 && numRead<bufSize)
    {
      buf[numRead]='\0';
      m_fileVersion=QCString(buf,numRead).stripWhiteSpace();
      if (!m_fileVersion.isEmpty())
      {
        msg("%s\n",m_fileVersion.data());
        return;
      }
    }
    msg("no version available\n");
  }
}


QCString FileDef::getSourceFileBase() const
{ 
  return convertNameToFile(m_diskName)+"_source"; 
}

/*! Returns the name of the verbatim copy of this file (if any). */
QCString FileDef::includeName() const 
{ 
  return getSourceFileBase();
}

bool FileDef::isLinkableInProject() const
{
  static bool showFiles = Config_getBool("SHOW_FILES");
  return hasDocumentation() && !isReference() && showFiles;
}

static void getAllIncludeFilesRecursively(
    QDict<void> *filesVisited,const FileDef *fd,QStrList &incFiles)
{
  if (fd->includeFileList())
  {
    QListIterator<IncludeInfo> iii(*fd->includeFileList());
    IncludeInfo *ii;
    for (iii.toFirst();(ii=iii.current());++iii)
    {
      if (ii->fileDef && !ii->fileDef->isReference() &&
          !filesVisited->find(ii->fileDef->absFilePath()))
      {
        //printf("FileDef::addIncludeDependency(%s)\n",ii->fileDef->absFilePath().data());
        incFiles.append(ii->fileDef->absFilePath());
        filesVisited->insert(ii->fileDef->absFilePath(),(void*)0x8);
        getAllIncludeFilesRecursively(filesVisited,ii->fileDef,incFiles);
      }
    }
  }
}

void FileDef::getAllIncludeFilesRecursively(QStrList &incFiles) const
{
  QDict<void> includes(257);
  ::getAllIncludeFilesRecursively(&includes,this,incFiles);
}

QCString FileDef::title() const
{
  return name();
}

QCString FileDef::fileVersion() const
{
  return m_fileVersion;
}
