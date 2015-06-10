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

Q_EXPORT bool qt_check_pointer( bool c, const char *, int );

#if defined(CHECK_NULL)
#define CHECK_PTR(p) (qt_check_pointer((p)==0,__FILE__,__LINE__))
#else
#define CHECK_PTR(p)
#endif
