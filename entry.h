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

#ifndef ENTRY_H
#define ENTRY_H

#include "types.h"

#include "qtools/qlist.h"
#include "qtools/qgstring.h"

struct SectionInfo;
class QFile;
class EntryNav;
class FileDef;
class FileStorage;
class StorageIntf;
class ArgumentList;
struct ListItemInfo;

/** This class stores information about an inheritance relation
 */ 
struct BaseInfo 
{
  /*! Creates an object representing an inheritance relation */
  BaseInfo(const char *n,Protection p,Specifier v) : 
    name(n),prot(p),virt(v) {}
  QCString   name; //!< the name of the base class
  Protection prot; //!< inheritance type
  Specifier  virt; //!< virtualness
};

/** This struct is used to capture the tag file information 
 *  for an Entry. 
 */
struct TagInfo 
{
  QCString tagName;
  QCString fileName;
  QCString anchor;
};

/** Represents an unstructured piece of information, about an
 *  entity found in the sources. 
 *
 *  parseMain() in scanner.l will generate a tree of these
 *  entries.
 */
class Entry
{
  public:

    /*! Kind of entries that are supported */
    enum Sections { 
      CLASS_SEC        = 0x00000001, 
      NAMESPACE_SEC    = 0x00000010,
      COMPOUND_MASK    = CLASS_SEC,
      SCOPE_MASK       = COMPOUND_MASK | NAMESPACE_SEC,
      
      CLASSDOC_SEC     = 0x00000800, 
      STRUCTDOC_SEC    = 0x00001000,
      UNIONDOC_SEC     = 0x00002000,
      EXCEPTIONDOC_SEC = 0x00004000,
      NAMESPACEDOC_SEC = 0x00008000,
      INTERFACEDOC_SEC = 0x00010000,
      PROTOCOLDOC_SEC  = 0x00020000,
      CATEGORYDOC_SEC  = 0x00040000,
      SERVICEDOC_SEC   = 0x00080000,
      SINGLETONDOC_SEC = 0x00100000,
      COMPOUNDDOC_MASK = CLASSDOC_SEC | STRUCTDOC_SEC | UNIONDOC_SEC | 
                         INTERFACEDOC_SEC | EXCEPTIONDOC_SEC | PROTOCOLDOC_SEC |
                         CATEGORYDOC_SEC | SERVICEDOC_SEC | SINGLETONDOC_SEC,

      SOURCE_SEC       = 0x00400000,
      HEADER_SEC       = 0x00800000,
      FILE_MASK        = SOURCE_SEC | HEADER_SEC,

      ENUMDOC_SEC      = 0x01000000,
      ENUM_SEC         = 0x02000000,
      EMPTY_SEC        = 0x03000000, 
      PAGEDOC_SEC      = 0x04000000, 
      VARIABLE_SEC     = 0x05000000,
      FUNCTION_SEC     = 0x06000000,
      TYPEDEF_SEC      = 0x07000000,
      MEMBERDOC_SEC    = 0x08000000, 
      OVERLOADDOC_SEC  = 0x09000000,
      EXAMPLE_SEC      = 0x0a000000, 
      VARIABLEDOC_SEC  = 0x0b000000,
      FILEDOC_SEC      = 0x0c000000,
      DEFINEDOC_SEC    = 0x0d000000,
      INCLUDE_SEC      = 0x0e000000,
      DEFINE_SEC       = 0x0f000000,
      GROUPDOC_SEC     = 0x10000000,
      USINGDIR_SEC     = 0x11000000,
      MAINPAGEDOC_SEC  = 0x12000000,
      MEMBERGRP_SEC    = 0x13000000,
      USINGDECL_SEC    = 0x14000000,
      PACKAGE_SEC      = 0x15000000,
      PACKAGEDOC_SEC   = 0x16000000,
      OBJCIMPL_SEC     = 0x17000000,
      DIRDOC_SEC       = 0x18000000
      ,EXPORTED_INTERFACE_SEC = 0x19000000
      ,INCLUDED_SERVICE_SEC = 0x1A000000
    };

    // class specifiers (add new items to the end)
    static const uint64 Template;
    static const uint64 Generic;
    static const uint64 Ref;
    static const uint64 Value;
    static const uint64 Interface;
    static const uint64 Struct;
    static const uint64 Union;
    static const uint64 Exception;
    static const uint64 Protocol;
    static const uint64 Category;
    static const uint64 SealedClass;
    static const uint64 AbstractClass;
    static const uint64 Enum; // for Java-style enums
    static const uint64 Service; // UNO IDL
    static const uint64 Singleton; // UNO IDL
    static const uint64 ForwardDecl; // forward declarad template classes

    // member specifiers (add new items to the beginning)
    static const uint64 PrivateGettable; // C# private getter
    static const uint64 ProtectedGettable; // C# protected getter
    static const uint64 PrivateSettable; // C# private setter
    static const uint64 ProtectedSettable; // C# protected setter
    static const uint64 Inline;
    static const uint64 Explicit;
    static const uint64 Mutable;
    static const uint64 Settable;
    static const uint64 Gettable;
    static const uint64 Readable;
    static const uint64 Writable;
    static const uint64 Final;
    static const uint64 Abstract;
    static const uint64 Addable;
    static const uint64 Removable;
    static const uint64 Raisable;
    static const uint64 Override;
    static const uint64 New;
    static const uint64 Sealed;
    static const uint64 Initonly;
    static const uint64 Optional;
    static const uint64 Required;
    static const uint64 NonAtomic;
    static const uint64 Copy;
    static const uint64 Retain;
    static const uint64 Assign;
    static const uint64 Strong;
    static const uint64 Weak;
    static const uint64 Unretained;
    static const uint64 Alias;
    static const uint64 ConstExp;
    static const uint64 Default;
    static const uint64 Delete;
    static const uint64 NoExcept;
    static const uint64 Attribute; // UNO IDL attribute
    static const uint64 Property; // UNO IDL property
    static const uint64 Readonly; // on UNO IDL attribute or property
    static const uint64 Bound; // on UNO IDL attribute or property
    static const uint64 Constrained; // on UNO IDL property
    static const uint64 Transient; // on UNO IDL property
    static const uint64 MaybeVoid; // on UNO IDL property
    static const uint64 MaybeDefault; // on UNO IDL property
    static const uint64 MaybeAmbiguous; // on UNO IDL property
    static const uint64 Published; // UNO IDL keyword

    enum GroupDocType
    {
      GROUPDOC_NORMAL,        //!< defgroup
      GROUPDOC_ADD,           //!< addgroup
      GROUPDOC_WEAK           //!< weakgroup
    };                        //!< kind of group

    Entry();
    Entry(const Entry &);
   ~Entry();

    /*! Returns the static size of the Entry (so excluding any dynamic memory) */
    int getSize();

    void addSpecialListItem(const char *listName,int index);
    void createNavigationIndex(EntryNav *rootNav,FileStorage *storage,FileDef *fd);

    // while parsing a file these function can be used to navigate/build the tree
    void setParent(Entry *parent) { m_parent = parent; }

    /*! Returns the parent for this Entry or 0 if this entry has no parent. */
    Entry *parent() const { return m_parent; }

    /*! Returns the list of children for this Entry 
     *  @see addSubEntry() and removeSubEntry()
     */
    const QList<Entry> *children() const { return m_sublist; }

    /*! Adds entry \a e as a child to this entry */
    void addSubEntry (Entry* e) ;

    /*! Removes entry \a e from the list of children. 
     *  Returns a pointer to the entry or 0 if the entry was not a child. 
     *  Note the entry will not be deleted.
     */ 
    Entry *removeSubEntry(Entry *e);

    /*! Restore the state of this Entry to the default value it has
     *  at construction time. 
     */
    void reset();

    /*! Serialize this entry to a persistent storage stream. */
    void marshall(StorageIntf *);

    /*! Reinitialize this entry from a persistent storage stream. */
    void unmarshall(StorageIntf *);

  public:

    // identification
    int          section;     //!< entry type (see Sections);
    QCString	 type;        //!< member type 
    QCString	 name;        //!< member name
    TagInfo     *tagInfo;     //!< tag file info

    // content
    Protection protection;    //!< class protection
    MethodTypes mtype;        //!< signal, slot, (dcop) method, or property?
    uint64 spec;              //!< class/member specifiers
    int  initLines;           //!< define/variable initializer lines to show 
    bool stat;                //!< static ?
    bool explicitExternal;    //!< explicitly defined as external?
    bool proto;               //!< prototype ?
    bool subGrouping;         //!< automatically group class members?
    bool callGraph;           //!< do we need to draw the call graph?
    bool callerGraph;         //!< do we need to draw the caller graph?
    Specifier    virt;        //!< virtualness of the entry 
    QCString     args;        //!< member argument string
    QCString     bitfields;   //!< member's bit fields
    ArgumentList *argList;    //!< member arguments as a list
    QList<ArgumentList> *tArgLists; //!< template argument declarations
    QGString	 program;     //!< the program text
    QGString     initializer; //!< initial value (for variables)
    QCString     includeFile; //!< include file (2 arg of \\class, must be unique)
    QCString     includeName; //!< include name (3 arg of \\class)
    QCString     doc;         //!< documentation block (partly parsed)
    int          docLine;     //!< line number at which the documentation was found
    QCString     docFile;     //!< file in which the documentation was found
    QCString     brief;       //!< brief description (doc block)
    int          briefLine;   //!< line number at which the brief desc. was found
    QCString     briefFile;   //!< file in which the brief desc. was found
    QCString     inbodyDocs;  //!< documentation inside the body of a function
    int          inbodyLine;  //!< line number at which the body doc was found
    QCString     inbodyFile;  //!< file in which the body doc was found
    QCString     relates;     //!< related class (doc block)
    RelatesType  relatesType; //!< how relates is handled
    QCString     read;        //!< property read accessor
    QCString     write;       //!< property write accessor
    QCString     inside;      //!< name of the class in which documents are found
    QCString     exception;   //!< throw specification
    ArgumentList *typeConstr; //!< where clause (C#) for type constraints
    int          bodyLine;    //!< line number of the definition in the source
    int          endBodyLine; //!< line number where the definition ends
    int          mGrpId;      //!< member group id
    QList<BaseInfo> *extends; //!< list of base classes    
    QList<Grouping> *groups;  //!< list of groups this entry belongs to
    QList<SectionInfo> *anchors; //!< list of anchors defined in this entry
    QCString	fileName;     //!< file this entry was extracted from
    int		startLine;    //!< start line of entry in the source
    int		startColumn;  //!< start column of entry in the source
    QList<ListItemInfo> *sli; //!< special lists (test/todo/bug/deprecated/..) this entry is in
    SrcLangExt  lang;         //!< programming language in which this entry was found
    bool        hidden;       //!< does this represent an entity that is hidden from the output
    bool        artificial;   //!< Artificially introduced item
    GroupDocType groupDocType;
    QCString    id;           //!< libclang id


    static int  num;          //!< counts the total number of entries

    /// return the command name used to define GROUPDOC_SEC
    const char *groupDocCmd() const
    {
      switch( groupDocType ) 
      {
        case GROUPDOC_NORMAL: return "\\defgroup";
        case GROUPDOC_ADD: return "\\addgroup";
        case GROUPDOC_WEAK: return "\\weakgroup";
        default: return "unknown group command";
      }
    }
    Grouping::GroupPri_t groupingPri() const
    {
      if( section != GROUPDOC_SEC ) 
      {
        return Grouping::GROUPING_LOWEST;
      }
      switch( groupDocType ) 
      {
        case GROUPDOC_NORMAL: return Grouping::GROUPING_AUTO_DEF;
        case GROUPDOC_ADD:    return Grouping::GROUPING_AUTO_ADD;
        case GROUPDOC_WEAK:   return Grouping::GROUPING_AUTO_WEAK;
        default: return Grouping::GROUPING_LOWEST;
      }
    }

  private:  
    void createSubtreeIndex(EntryNav *nav,FileStorage *storage,FileDef *fd);
    Entry         *m_parent;    //!< parent node in the tree
    QList<Entry>  *m_sublist;   //!< entries that are children of this one
    Entry &operator=(const Entry &); 
};

/** Wrapper for a node in the Entry tree.
 *
 *  Allows navigating through the Entry tree and load and storing Entry
 *  objects persistently to disk.
 */
class EntryNav
{
  public:
    EntryNav(EntryNav *parent,Entry *e);
   ~EntryNav();
    void addChild(EntryNav *);
    bool loadEntry(FileStorage *storage);
    bool saveEntry(Entry *e,FileStorage *storage);
    void setEntry(Entry *e);
    void releaseEntry();
    void changeSection(int section) { m_section = section; }
    void setFileDef(FileDef *fd) { m_fileDef = fd; }

    Entry *entry() const { return m_info; }
    int section() const { return m_section; }
    SrcLangExt lang() const { return m_lang; }
    const QCString &type() const { return m_type; }
    const QCString &name() const { return m_name; }
    TagInfo *tagInfo() const { return m_tagInfo; }
    const QList<EntryNav> *children() const { return m_subList; }
    EntryNav *parent() const { return m_parent; }
    FileDef *fileDef() const { return m_fileDef; }

  private:

    // navigation 
    EntryNav        *m_parent;    //!< parent node in the tree
    QList<EntryNav> *m_subList;   //!< entries that are children of this one

    // identification
    int          m_section;     //!< entry type (see Sections);
    QCString	 m_type;        //!< member type 
    QCString	 m_name;        //!< member name
    TagInfo     *m_tagInfo;      //!< tag file info
    FileDef     *m_fileDef;
    SrcLangExt   m_lang;         //!< programming language in which this entry was found

    Entry       *m_info;
    int64        m_offset;
    bool         m_noLoad;
};


typedef QList<Entry> EntryList;
typedef QListIterator<Entry> EntryListIterator;

typedef QList<EntryNav> EntryNavList;
typedef QListIterator<EntryNav> EntryNavListIterator;

#endif
