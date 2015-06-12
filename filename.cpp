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
#include "qtools/qcstring.h"
#include "qtools/qdir.h"
#include "filename.h"
#include "util.h"
#include "config.h"

FileName::FileName(const char *fn,const char *n) : FileList()
{
  setAutoDelete(TRUE);
  fName=fn;
  name=n;
}

FileName::~FileName()
{
}


void FileName::generateDiskNames()
{
}

FileNameList::FileNameList() : QList<FileName>()
{
}

FileNameList::~FileNameList()
{
}

void FileNameList::generateDiskNames()
{
  FileNameListIterator it(*this);
  FileName *fn;
  for (;(fn=it.current());++it)
  {
    fn->generateDiskNames();
  }
}

int FileNameList::compareValues(const FileName *f1, const FileName *f2) const
{
  return Config_getBool("FULL_PATH_NAMES") ?
         qstricmp(f1->fullName(),f2->fullName()) :
         qstricmp(f1->fileName(),f2->fileName());
}

FileNameListIterator::FileNameListIterator(const FileNameList &fnlist) :
  QListIterator<FileName>(fnlist)
{
}

static bool getCaseSenseNames()
{
  static bool caseSenseNames = false;
  return caseSenseNames;
}

FileNameDict::FileNameDict(uint size) : QDict<FileName>(size,getCaseSenseNames()) 
{
}

