#include "StdAfx.h"
#include "qtools/qcstring.h"
#include "qtools/qdir.h"
#include "doxygen.h"

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
}
