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

#ifndef OUTPUTGEN_H
#define OUTPUTGEN_H

#include "qtools/qstack.h"

struct DocLinkInfo
{
  QCString name;
  QCString ref;
  QCString url;
  QCString anchor;
};

struct SourceLinkInfo
{
  QCString file;
  int line;
  QCString ref;
  QCString url;
  QCString anchor;
};

/** Output interface for code parser. 
 */
class CodeOutputInterface
{
  public:
    virtual ~CodeOutputInterface() {}

    /*! Writes an code fragment to the output. This function should keep 
     *  spaces visible, should break lines at a newline and should convert 
     *  tabs to the right number of spaces.
     */
    virtual void codify(const char *s) = 0;

    /*! Writes a link to an object in a code fragment.
     *  \param ref      If this is non-zero, the object is to be found in
     *                  an external documentation file.
     *  \param file     The file in which the object is located.
     *  \param anchor   The anchor uniquely identifying the object within 
     *                  the file. 
     *  \param name     The text to display as a placeholder for the link.
     *  \param tooltip  The tooltip to display when the mouse is on the link.
     */
    virtual void writeCodeLink(const char *ref,const char *file,
                               const char *anchor,const char *name,
                               const char *tooltip) = 0;

    /*! Writes the line number of a source listing
     *  \param ref        External reference (when imported from a tag file)
     *  \param file       The file part of the URL pointing to the docs.
     *  \param anchor     The anchor part of the URL pointing to the docs.
     *  \param lineNumber The line number to write
     */
    virtual void writeLineNumber(const char *ref,const char *file,
                                 const char *anchor,int lineNumber) = 0;

    /*! Writes a tool tip definition 
     *  \param id       unique identifier for the tooltip
     *  \param docInfo  Info about the symbol's documentation.
     *  \param decl     full declaration of the symbol (for functions)
     *  \param desc     brief description for the symbol
     *  \param defInfo  Info about the symbol's definition in the source code
     *  \param declInfo Info about the symbol's declaration in the source code
     */
    virtual void writeTooltip(const char *id, 
                              const DocLinkInfo &docInfo,
                              const char *decl,
                              const char *desc,
                              const SourceLinkInfo &defInfo,
                              const SourceLinkInfo &declInfo
                             ) = 0;
                              
    virtual void startCodeLine(bool hasLineNumbers) = 0;

    /*! Ends a line of code started with startCodeLine() */
    virtual void endCodeLine() = 0;

    /*! Starts a block with a certain meaning. Used for syntax highlighting,
     *  which elements of the same type are rendered using the same 'font class'.
     *  \param clsName The category name.
     */
    virtual void startFontClass(const char *clsName) = 0;

    /*! Ends a block started with startFontClass() */
    virtual void endFontClass() = 0;

    /*! Write an anchor to a source listing.
     *  \param name The name of the anchor.
     */
    virtual void writeCodeAnchor(const char *name) = 0;

    virtual void setCurrentDoc(Definition *context,const char *anchor,bool isSourceFile) = 0;
    virtual void addWord(const char *word,bool hiPriority) = 0;
};

#endif
