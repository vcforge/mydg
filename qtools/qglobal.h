// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the QTOOLS_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// QTOOLS_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef QTOOLS_EXPORTS
#define Q_EXPORT __declspec(dllexport)
#else
#define Q_EXPORT __declspec(dllimport)
#endif

Q_EXPORT bool qSysInfo( int *wordSize, bool *bigEndian );

typedef int intptr_t;

typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned	uint;
typedef unsigned long	ulong;
typedef char	       *pchar;
typedef uchar	       *puchar;
typedef const char     *pcchar;

typedef signed char	Q_INT8;			// 8 bit signed
typedef unsigned char	Q_UINT8;		// 8 bit unsigned
typedef short		Q_INT16;		// 16 bit signed
typedef unsigned short	Q_UINT16;		// 16 bit unsigned
typedef int		Q_INT32;		// 32 bit signed
typedef unsigned int	Q_UINT32;		// 32 bit unsigned
typedef long		Q_INT64;		// up to 64 bit signed
typedef unsigned long	Q_UINT64;		// up to 64 bit unsigned

//
// Use to avoid "unused parameter" warnings
//

#define Q_UNUSED(x) x=x;
#define Q_CONST_UNUSED(x) (void)x;

Q_EXPORT void qWarning( const char *, ... );	// print warning message

Q_EXPORT void qFatal( const char *, ... );	// print fatal message and exit

#if defined(CHECK_NULL)
#define CHECK_PTR(p) (qt_check_pointer((p)==0,__FILE__,__LINE__))
#else
#define CHECK_PTR(p)
#endif
