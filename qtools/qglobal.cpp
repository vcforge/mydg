#include "StdAfx.h"
#include "qglobal.h"

void qFatal( const char *msg, ... )
{
    exit(0);
}

//
// The CHECK_PTR macro calls this function to check if an allocation went ok.
//

bool qt_check_pointer( bool c, const char *n, int l )
{
    if ( c )
	qFatal( "In file %s, line %d: Out of memory", n, l );
    return TRUE;
}

