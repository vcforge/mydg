#include "StdAfx.h"
#include "qglobal.h"

/*****************************************************************************
  System detection routines
 *****************************************************************************/

static bool si_alreadyDone = FALSE;
static int  si_wordSize;
static bool si_bigEndian;

/*!
  \relates QApplication
  Obtains information about the system.

  The system's word size in bits (typically 32) is returned in \e *wordSize.
  The \e *bigEndian is set to TRUE if this is a big-endian machine,
  or to FALSE if this is a little-endian machine.

  This function calls qFatal() with a message if the computer is truly weird
  (i.e. different endianness for 16 bit and 32 bit integers).
*/

bool qSysInfo( int *wordSize, bool *bigEndian )
{
#if defined(CHECK_NULL)
    ASSERT( wordSize != 0 );
    ASSERT( bigEndian != 0 );
#endif

    if ( si_alreadyDone ) {			// run it only once
	*wordSize  = si_wordSize;
	*bigEndian = si_bigEndian;
	return TRUE;
    }
    si_alreadyDone = TRUE;

    si_wordSize = 0;
    uint n = (uint)(~0);
    while ( n ) {				// detect word size
	si_wordSize++;
	n /= 2;
    }
    *wordSize = si_wordSize;

    if ( *wordSize != 64 &&
	 *wordSize != 32 &&
	 *wordSize != 16 ) {			// word size: 16, 32 or 64
#if defined(CHECK_RANGE)
	qFatal( "qSysInfo: Unsupported system word size %d", *wordSize );
#endif
	return FALSE;
    }
    if ( sizeof(Q_INT8) != 1 || sizeof(Q_INT16) != 2 || sizeof(Q_INT32) != 4 ||
	 sizeof(float) != 4 || sizeof(double) != 8 ) {
#if defined(CHECK_RANGE)
	qFatal( "qSysInfo: Unsupported system data type size" );
#endif
	return FALSE;
    }

    bool  be16, be32;				// determine byte ordering
    short ns = 0x1234;
    int	  nl = 0x12345678;

    unsigned char *p = (unsigned char *)(&ns);	// 16-bit integer
    be16 = *p == 0x12;

    p = (unsigned char *)(&nl);			// 32-bit integer
    if ( p[0] == 0x12 && p[1] == 0x34 && p[2] == 0x56 && p[3] == 0x78 )
	be32 = TRUE;
    else
    if ( p[0] == 0x78 && p[1] == 0x56 && p[2] == 0x34 && p[3] == 0x12 )
	be32 = FALSE;
    else
	be32 = !be16;

    if ( be16 != be32 ) {			// strange machine!
#if defined(CHECK_RANGE)
	qFatal( "qSysInfo: Inconsistent system byte order" );
#endif
	return FALSE;
    }

    *bigEndian = si_bigEndian = be32;
    return TRUE;
}

void qWarning( const char *msg, ... )
{
}

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

