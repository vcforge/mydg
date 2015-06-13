# Microsoft Developer Studio Project File - Name="qtools" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=qtools - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "qtools.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "qtools.mak" CFG="qtools - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "qtools - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "qtools - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "qtools - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTOOLS_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTOOLS_EXPORTS" /D "_OS_WIN32_" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386

!ELSEIF  "$(CFG)" == "qtools - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTOOLS_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "QTOOLS_EXPORTS" /D "_OS_WIN32_" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"../Debug/qtools.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "qtools - Win32 Release"
# Name "qtools - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\qbuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\qcollection.cpp
# End Source File
# Begin Source File

SOURCE=.\qcstring.cpp
# End Source File
# Begin Source File

SOURCE=.\qdatastream.cpp
# End Source File
# Begin Source File

SOURCE=.\qdatetime.cpp
# End Source File
# Begin Source File

SOURCE=.\qdir.cpp
# End Source File
# Begin Source File

SOURCE=.\qdir_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\qfile.cpp
# End Source File
# Begin Source File

SOURCE=.\qfile_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\qfileinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\qfileinfo_win32.cpp
# End Source File
# Begin Source File

SOURCE=.\qgarray.cpp
# End Source File
# Begin Source File

SOURCE=.\qgcache.cpp
# End Source File
# Begin Source File

SOURCE=.\qgdict.cpp
# End Source File
# Begin Source File

SOURCE=.\qglist.cpp
# End Source File
# Begin Source File

SOURCE=.\qglobal.cpp
# End Source File
# Begin Source File

SOURCE=.\qgstring.cpp
# End Source File
# Begin Source File

SOURCE=.\qgvector.cpp
# End Source File
# Begin Source File

SOURCE=.\qiodevice.cpp
# End Source File
# Begin Source File

SOURCE=.\qregexp.cpp
# End Source File
# Begin Source File

SOURCE=.\qstring.cpp
# End Source File
# Begin Source File

SOURCE=.\qstringlist.cpp
# End Source File
# Begin Source File

SOURCE=.\qtextcodec.cpp
# End Source File
# Begin Source File

SOURCE=.\qtextstream.cpp
# End Source File
# Begin Source File

SOURCE=.\qtools.cpp
# End Source File
# Begin Source File

SOURCE=.\qutfcodec.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\qarray.h
# End Source File
# Begin Source File

SOURCE=.\qasciidict.h
# End Source File
# Begin Source File

SOURCE=.\qbuffer.h
# End Source File
# Begin Source File

SOURCE=.\qcache.h
# End Source File
# Begin Source File

SOURCE=.\qcollection.h
# End Source File
# Begin Source File

SOURCE=.\qcstring.h
# End Source File
# Begin Source File

SOURCE=.\qdatastream.h
# End Source File
# Begin Source File

SOURCE=.\qdatetime.h
# End Source File
# Begin Source File

SOURCE=.\qdict.h
# End Source File
# Begin Source File

SOURCE=.\qdir.h
# End Source File
# Begin Source File

SOURCE=.\qfile.h
# End Source File
# Begin Source File

SOURCE=.\qfiledefs_p.h
# End Source File
# Begin Source File

SOURCE=.\qfileinfo.h
# End Source File
# Begin Source File

SOURCE=.\qgarray.h
# End Source File
# Begin Source File

SOURCE=.\qgcache.h
# End Source File
# Begin Source File

SOURCE=.\qgdict.h
# End Source File
# Begin Source File

SOURCE=.\qglist.h
# End Source File
# Begin Source File

SOURCE=.\qglobal.h
# End Source File
# Begin Source File

SOURCE=.\qgstring.h
# End Source File
# Begin Source File

SOURCE=.\qgvector.h
# End Source File
# Begin Source File

SOURCE=.\qintdict.h
# End Source File
# Begin Source File

SOURCE=.\qiodevice.h
# End Source File
# Begin Source File

SOURCE=.\qlist.h
# End Source File
# Begin Source File

SOURCE=.\qqueue.h
# End Source File
# Begin Source File

SOURCE=.\qregexp.h
# End Source File
# Begin Source File

SOURCE=.\qshared.h
# End Source File
# Begin Source File

SOURCE=.\qstack.h
# End Source File
# Begin Source File

SOURCE=.\qstring.h
# End Source File
# Begin Source File

SOURCE=.\qstringlist.h
# End Source File
# Begin Source File

SOURCE=.\qstrlist.h
# End Source File
# Begin Source File

SOURCE=.\qtextcodec.h
# End Source File
# Begin Source File

SOURCE=.\qtextstream.h
# End Source File
# Begin Source File

SOURCE=.\qtl.h
# End Source File
# Begin Source File

SOURCE=.\qutfcodec.h
# End Source File
# Begin Source File

SOURCE=.\qvaluelist.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
