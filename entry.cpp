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
#include "qtools/qfile.h"
#include "qtools/qstrlist.h"
#include "entry.h"
#include "util.h"
#include "section.h"
#include "arguments.h"

//------------------------------------------------------------------

#define HEADER ('D'<<24)+('O'<<16)+('X'<<8)+'!'

//------------------------------------------------------------------

int Entry::num=0;

// class specifiers (add new items to the end)
const uint64 Entry::Template        = (1<<0);
const uint64 Entry::Generic         = (1<<1);
const uint64 Entry::Ref             = (1<<2);
const uint64 Entry::Value           = (1<<3);
const uint64 Entry::Interface       = (1<<4);
const uint64 Entry::Struct          = (1<<5);
const uint64 Entry::Union           = (1<<6);
const uint64 Entry::Exception       = (1<<7);
const uint64 Entry::Protocol        = (1<<8);
const uint64 Entry::Category        = (1<<9);
const uint64 Entry::SealedClass     = (1<<10);
const uint64 Entry::AbstractClass   = (1<<11);
const uint64 Entry::Enum            = (1<<12); // for Java-style enums
const uint64 Entry::Service         = (1<<13); // UNO IDL
const uint64 Entry::Singleton       = (1<<14); // UNO IDL
const uint64 Entry::ForwardDecl     = (1<<15); // forward declarad template classes

// member specifiers (add new items to the beginning)
const uint64 Entry::PrivateGettable     = (1<<20); // C# private getter
const uint64 Entry::ProtectedGettable   = (1<<21); // C# protected getter
const uint64 Entry::PrivateSettable     = (1<<22); // C# private setter
const uint64 Entry::ProtectedSettable   = (1<<23); // C# protected setter
const uint64 Entry::Inline          = (1<<24);
const uint64 Entry::Explicit        = (1<<25);
const uint64 Entry::Mutable         = (1<<26);
const uint64 Entry::Settable        = (1<<27);
const uint64 Entry::Gettable        = (1<<28);
const uint64 Entry::Readable        = (1<<29);
const uint64 Entry::Writable        = (1<<30);
const uint64 Entry::Final           = (1<<31);
const uint64 Entry::Abstract        = (1<<32);
const uint64 Entry::Addable         = (1<<33);
const uint64 Entry::Removable       = (1<<34);
const uint64 Entry::Raisable        = (1<<35);
const uint64 Entry::Override        = (1<<36);
const uint64 Entry::New             = (1<<37);
const uint64 Entry::Sealed          = (1<<38);
const uint64 Entry::Initonly        = (1<<39);
const uint64 Entry::Optional        = (1<<40);
const uint64 Entry::Required        = (1<<41);
const uint64 Entry::NonAtomic       = (1<<42);
const uint64 Entry::Copy            = (1<<43);
const uint64 Entry::Retain          = (1<<44);
const uint64 Entry::Assign          = (1<<45);
const uint64 Entry::Strong          = (1<<46);
const uint64 Entry::Weak            = (1<<47);
const uint64 Entry::Unretained      = (1<<48);
const uint64 Entry::Alias           = (1<<49);
const uint64 Entry::ConstExp        = (1<<50);
const uint64 Entry::Default         = (1<<51);
const uint64 Entry::Delete          = (1<<52);
const uint64 Entry::NoExcept        = (1<<53);
const uint64 Entry::Attribute       = (1<<54); // UNO IDL attribute
const uint64 Entry::Property        = (1<<55); // UNO IDL property
const uint64 Entry::Readonly        = (1<<56); // on UNO IDL attribute or property
const uint64 Entry::Bound           = (1<<57); // on UNO IDL attribute or property
const uint64 Entry::Constrained     = (1<<58); // on UNO IDL property
const uint64 Entry::Transient       = (1<<59); // on UNO IDL property
const uint64 Entry::MaybeVoid       = (1<<60); // on UNO IDL property
const uint64 Entry::MaybeDefault    = (1<<61); // on UNO IDL property
const uint64 Entry::MaybeAmbiguous  = (1<<62); // on UNO IDL property
const uint64 Entry::Published       = (1<<63); // UNO IDL keyword

Entry::Entry()
{
  //printf("Entry::Entry(%p)\n",this);
  num++;
  m_parent=0;
  section = EMPTY_SEC;
  m_sublist = new QList<Entry>;
  m_sublist->setAutoDelete(TRUE);
  extends = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors = new QList<SectionInfo>; // Doxygen::sectionDict takes ownership of the items!
  argList = new ArgumentList;
  argList->setAutoDelete(TRUE);
  //printf("Entry::Entry() tArgList=0\n");
  tArgLists = 0;
  typeConstr = 0;
  mGrpId = -1;
  tagInfo = 0;
  sli = 0;
  relatesType = Simple;
  hidden = FALSE;
  groupDocType = GROUPDOC_NORMAL;
  reset();
}

Entry::Entry(const Entry &e)
{
  //printf("Entry::Entry(%p):copy\n",this);
  num++;
  section     = e.section;
  type        = e.type;
  name        = e.name;
  tagInfo     = e.tagInfo;
  protection  = e.protection;
  mtype       = e.mtype;
  spec        = e.spec;
  initLines   = e.initLines;
  stat        = e.stat;
  explicitExternal = e.explicitExternal;
  proto       = e.proto;
  subGrouping = e.subGrouping;
  callGraph   = e.callGraph;
  callerGraph = e.callerGraph;
  virt        = e.virt;
  args        = e.args;
  bitfields   = e.bitfields;
  argList     = e.argList->deepCopy();
  tArgLists = 0;
  program     = e.program;
  initializer = e.initializer;
  includeFile = e.includeFile;
  includeName = e.includeName;
  doc         = e.doc;
  docLine     = e.docLine;
  docFile     = e.docFile;
  brief       = e.brief;
  briefLine   = e.briefLine;
  briefFile   = e.briefFile;
  inbodyDocs  = e.inbodyDocs;
  inbodyLine  = e.inbodyLine;
  inbodyFile  = e.inbodyFile;
  relates     = e.relates;
  relatesType = e.relatesType;
  read        = e.read;
  write       = e.write;
  inside      = e.inside;
  exception   = e.exception;
  typeConstr  = 0;
  bodyLine    = e.bodyLine;
  endBodyLine = e.endBodyLine;
  mGrpId      = e.mGrpId;
  extends     = new QList<BaseInfo>;
  extends->setAutoDelete(TRUE);
  groups      = new QList<Grouping>;
  groups->setAutoDelete(TRUE);
  anchors     = new QList<SectionInfo>;
  fileName    = e.fileName;
  startLine   = e.startLine;
  startColumn = e.startColumn;
  if (e.sli)
  {
    sli = new QList<ListItemInfo>;
    sli->setAutoDelete(TRUE);
    QListIterator<ListItemInfo> slii(*e.sli);
    ListItemInfo *ili;
    for (slii.toFirst();(ili=slii.current());++slii)
    {
      sli->append(new ListItemInfo(*ili));
    }
  }
  else
  {
    sli=0;
  }
  lang        = e.lang;
  hidden      = e.hidden;
  artificial  = e.artificial;
  groupDocType = e.groupDocType;
  id          = e.id;

  m_parent    = e.m_parent;
  m_sublist   = new QList<Entry>;
  m_sublist->setAutoDelete(TRUE);

  // deep copy of the child entry list
  QListIterator<Entry> eli(*e.m_sublist);
  Entry *cur;
  for (;(cur=eli.current());++eli)
  {
    m_sublist->append(new Entry(*cur));
  }
  
  // deep copy base class list
  QListIterator<BaseInfo> bli(*e.extends);
  BaseInfo *bi;
  for (;(bi=bli.current());++bli)
  {
    extends->append(new BaseInfo(*bi));
  }
  
  // deep copy group list
  QListIterator<Grouping> gli(*e.groups);
  Grouping *g;
  for (;(g=gli.current());++gli)
  {
    groups->append(new Grouping(*g));
  }
  
  QListIterator<SectionInfo> sli2(*e.anchors);
  SectionInfo *s;
  for (;(s=sli2.current());++sli2)
  {
    anchors->append(new SectionInfo(*s));
  }

  // deep copy type constraint list
  if (e.typeConstr)
  {
    typeConstr  = e.typeConstr->deepCopy();
  }

  // deep copy template argument lists
  if (e.tArgLists)
  {
//    tArgLists = copyArgumentLists(e.tArgLists);
  }

}

Entry::~Entry()
{
  //printf("Entry::~Entry(%p) num=%d\n",this,num);
  //printf("Deleting entry %d name %s type %x children %d\n",
  //       num,name.data(),section,sublist->count());
  
  delete m_sublist; // each element is now own by a EntryNav so we do no longer own
                  // our children.
  delete extends;
  delete groups;
  delete anchors;
  delete argList;
  delete tArgLists;
  delete tagInfo;
  delete typeConstr;
  delete sli;
  num--;
}

void Entry::addSubEntry(Entry *current)
{
  //printf("Entry %d with name %s type 0x%x added to %s type 0x%x\n",
  //    current->num,current->name.data(),current->section,
  //    name.data(),section);
  //printf("Entry::addSubEntry(%s:%p) to %s\n",current->name.data(),
  //    current,name.data());
  current->m_parent=this;
  m_sublist->append(current);  
}

void Entry::reset()
{
  //printf("Entry::reset()\n");
  name.resize(0);
  type.resize(0);
  args.resize(0);
  bitfields.resize(0);
  exception.resize(0);
  program.resize(0);
  includeFile.resize(0);
  includeName.resize(0);
  doc.resize(0);
  docFile.resize(0);
  docLine=-1;
  relates.resize(0);
  relatesType=Simple;
  brief.resize(0);
  briefFile.resize(0);
  briefLine=-1;
  inbodyDocs.resize(0);
  inbodyFile.resize(0);
  inbodyLine=-1;
  inside.resize(0);
  fileName.resize(0);
  initializer.resize(0);
  initLines = -1;
  startLine = 1;
  startColumn = 1;
  bodyLine = -1;
  endBodyLine = -1;
  mGrpId = -1;
  callGraph = FALSE;
  callerGraph = FALSE;
  section = EMPTY_SEC;
  mtype   = Method;
  virt    = Normal;
  stat    = FALSE;
  proto   = FALSE;
  explicitExternal = FALSE;
  spec  = 0;
  lang = SrcLangExt_Unknown;
  hidden = FALSE;
  artificial = FALSE;
  subGrouping = TRUE;
  protection = Public;
  groupDocType = GROUPDOC_NORMAL;
  id.resize(0);
  m_sublist->clear();
  extends->clear();
  groups->clear();
  anchors->clear();
  argList->clear();
  if (tagInfo)    { delete tagInfo; tagInfo=0; }
  if (tArgLists)  { delete tArgLists; tArgLists=0; }
  if (sli)        { delete sli; sli=0; }
  if (typeConstr) { delete typeConstr; typeConstr=0; }
  //if (mtArgList) { delete mtArgList; mtArgList=0; }
}


int Entry::getSize()
{
  return sizeof(Entry);
}

void Entry::createSubtreeIndex(EntryNav *nav,FileStorage *storage,FileDef *fd)
{
  EntryNav *childNav = new EntryNav(nav,this);
  nav->addChild(childNav);
  childNav->setFileDef(fd);
  childNav->saveEntry(this,storage);
  if (m_sublist)
  {
    //printf("saveEntry: %d children\n",node->sublist->count());
    QListIterator<Entry> eli(*m_sublist);
    Entry *childNode;
    for (eli.toFirst();(childNode=eli.current());++eli)
    {
      childNode->createSubtreeIndex(childNav,storage,fd);
    }
    //m_sublist->setAutoDelete(FALSE);
    m_sublist->clear();
  }
}

void Entry::createNavigationIndex(EntryNav *rootNav,FileStorage *storage,FileDef *fd)
{
  createSubtreeIndex(rootNav,storage,fd);
}

void Entry::addSpecialListItem(const char *listName,int itemId)
{
  if (sli==0)
  {
    sli = new QList<ListItemInfo>;
    sli->setAutoDelete(TRUE);
  }
  ListItemInfo *ili=new ListItemInfo;
  ili->type = listName;
  ili->itemId = itemId;
  sli->append(ili);
}

Entry *Entry::removeSubEntry(Entry *e)
{
 int i = m_sublist->find(e);
 return i!=-1 ? m_sublist->take(i) : 0;
}

//------------------------------------------------------------------


EntryNav::EntryNav(EntryNav *parent, Entry *e)
             : m_parent(parent), m_subList(0), m_section(e->section), m_type(e->type),
              m_name(e->name), m_fileDef(0), m_lang(e->lang), 
              m_info(0), m_offset(-1), m_noLoad(FALSE) 
{
  if (e->tagInfo)
  {
    m_tagInfo = new TagInfo;
    m_tagInfo->tagName  = e->tagInfo->tagName;
    m_tagInfo->fileName = e->tagInfo->fileName;
    m_tagInfo->anchor   = e->tagInfo->anchor;
    if (e->tagInfo)
    {
      //printf("tagInfo %p: tagName=%s fileName=%s anchor=%s\n",
      //    e->tagInfo,
      //    e->tagInfo->tagName.data(),
      //    e->tagInfo->fileName.data(),
      //    e->tagInfo->anchor.data());
    }
  }
  else
  {
    m_tagInfo = 0;
  }
}

EntryNav::~EntryNav()
{
  delete m_subList;
  delete m_info;
  delete m_tagInfo;
}

void EntryNav::addChild(EntryNav *e)
{
  if (m_subList==0) 
  {
    m_subList = new QList<EntryNav>;
    m_subList->setAutoDelete(TRUE);
  }
  m_subList->append(e);
}

bool EntryNav::loadEntry(FileStorage *storage)
{
/*  if (m_noLoad)
  {
    return TRUE;
  }
  if (m_offset==-1) 
  {
    //printf("offset not set!\n");
    return FALSE;
  }
  //delete m_info;
  //printf("EntryNav::loadEntry: new entry %p: %s\n",m_info,m_name.data());
  //m_info->tagInfo = m_tagInfo;
  //if (m_parent)
  //{
  //  m_info->parent = m_parent->m_info;
  //}
  //m_info->parent = 0;
  //printf("load entry: seek to %llx\n",m_offset);
  if (!storage->seek(m_offset)) 
  {
    //printf("seek failed!\n");
    return FALSE;
  }
  if (m_info)  delete m_info;
  m_info = unmarshalEntry(storage);
  m_info->name = m_name;
  m_info->type = m_type;
  m_info->section = m_section;
*/  return TRUE;
}

bool EntryNav::saveEntry(Entry *e,FileStorage *storage)
{
/*  m_offset = storage->pos();
  //printf("EntryNav::saveEntry offset=%llx\n",m_offset);
  marshalEntry(storage,e);
*/  return TRUE;
}

void EntryNav::releaseEntry()
{
  if (!m_noLoad) 
  { 
    //printf("EntryNav::releaseEntry %p\n",m_info);
    delete m_info; 
    m_info=0; 
  }
}

void EntryNav::setEntry(Entry *e) 
{ 
  delete m_info;
  m_info = e; 
  //printf("EntryNav::setEntry %p\n",e);
  m_noLoad=TRUE; 
}

