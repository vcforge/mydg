#ifndef QGSTRING_H
#define QGSTRING_H

#include <stdlib.h>
#include <string.h>

#if defined(_OS_SUN_) && defined(_CC_GNU_)
#include <strings.h>
#endif

/*****************************************************************************
  Safe and portable C string functions; extensions to standard string.h
 *****************************************************************************/

Q_EXPORT void *qmemmove( void *dst, const void *src, uint len );

#if defined(_OS_SUN_) || defined(_CC_OC_)
#define memmove(s1,s2,n) qmemmove((s1),(s2),(n))
#endif

#if defined(_OS_WIN32_)
#define qsnprintf _snprintf
#else
#define qsnprintf snprintf
#endif

Q_EXPORT char *qstrdup( const char * );

Q_EXPORT inline uint cstrlen( const char *str )
{ return (uint)strlen(str); }

Q_EXPORT inline uint qstrlen( const char *str )
{ return str ? (uint)strlen(str) : 0; }

Q_EXPORT inline char *cstrcpy( char *dst, const char *src )
{ return strcpy(dst,src); }

Q_EXPORT inline char *qstrcpy( char *dst, const char *src )
{ return src ? strcpy(dst, src) : 0; }

Q_EXPORT char * qstrncpy(char *src,const char *dst, uint len);

Q_EXPORT inline int cstrcmp( const char *str1, const char *str2 )
{ return strcmp(str1,str2); }

Q_EXPORT inline int qstrcmp( const char *str1, const char *str2 )
{ return (str1 && str2) ? strcmp(str1,str2) : (int)((intptr_t)str2 - (intptr_t)str1); }

Q_EXPORT inline int cstrncmp( const char *str1, const char *str2, uint len )
{ return strncmp(str1,str2,len); }

Q_EXPORT inline int qstrncmp( const char *str1, const char *str2, uint len )
{ return (str1 && str2) ? strncmp(str1,str2,len) :
			  (int)((intptr_t)str2 - (intptr_t)str1); }

Q_EXPORT int qstricmp( const char *str1, const char *str2 );

Q_EXPORT int qstrnicmp( const char *str1, const char *str2, uint len );

/*****************************************************************************
  Fixes and workarounds for some platforms
 *****************************************************************************/

/** This is an alternative implementation of QCString. 
 */
class QGString 
{
  public:
    QGString(); // make null string
    QGString(uint size); 
    QGString( const QGString &s );
    QGString( const char *str );
   ~QGString() ;

    bool resize( uint newlen );
    bool enlarge( uint newlen );
    void setLen( uint newlen );

    QGString    &operator=( const QGString &s );
    QGString    &operator=( const char *str );
    QGString    &operator+=( const QGString &s );
    QGString    &operator+=( const char *str );
    QGString    &operator+=( char c );

    bool        isNull()        const { return m_data==0; }
    bool	isEmpty()	const { return m_len==0; }
    uint	length()	const { return m_len; }
    uint        size()          const { return m_memSize; }
    char *      data()          const { return m_data; }
    bool	truncate( uint pos )  { return resize(pos+1); }
        operator const char *() const { return (const char *)data(); }
    char &at( uint index ) const      { return m_data[index]; }
    char &operator[]( int i ) const   { return at(i); }
    
  private:
    char *      m_data;
    uint        m_len;
    uint        m_memSize;
};

/*****************************************************************************
  QGString non-member operators
 *****************************************************************************/

Q_EXPORT inline bool operator==( const QGString &s1, const QGString &s2 )
{ return qstrcmp(s1.data(),s2.data()) == 0; }

Q_EXPORT inline bool operator==( const QGString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) == 0; }

Q_EXPORT inline bool operator==( const char *s1, const QGString &s2 )
{ return qstrcmp(s1,s2.data()) == 0; }

Q_EXPORT inline bool operator!=( const QGString &s1, const QGString &s2 )
{ return qstrcmp(s1.data(),s2.data()) != 0; }

Q_EXPORT inline bool operator!=( const QGString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) != 0; }

Q_EXPORT inline bool operator!=( const char *s1, const QGString &s2 )
{ return qstrcmp(s1,s2.data()) != 0; }

Q_EXPORT inline bool operator<( const QGString &s1, const QGString& s2 )
{ return qstrcmp(s1.data(),s2.data()) < 0; }

Q_EXPORT inline bool operator<( const QGString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) < 0; }

Q_EXPORT inline bool operator<( const char *s1, const QGString &s2 )
{ return qstrcmp(s1,s2.data()) < 0; }

Q_EXPORT inline bool operator<=( const QGString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) <= 0; }

Q_EXPORT inline bool operator<=( const char *s1, const QGString &s2 )
{ return qstrcmp(s1,s2.data()) <= 0; }

Q_EXPORT inline bool operator>( const QGString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) > 0; }

Q_EXPORT inline bool operator>( const char *s1, const QGString &s2 )
{ return qstrcmp(s1,s2.data()) > 0; }

Q_EXPORT inline bool operator>=( const QGString &s1, const char *s2 )
{ return qstrcmp(s1.data(),s2) >= 0; }

Q_EXPORT inline bool operator>=( const char *s1, const QGString &s2 )
{ return qstrcmp(s1,s2.data()) >= 0; }

Q_EXPORT inline QGString operator+( const QGString &s1, const QGString &s2 )
{
    QGString tmp( s1.data() );
    tmp += s2;
    return tmp;
}

Q_EXPORT inline QGString operator+( const QGString &s1, const char *s2 )
{
    QGString tmp( s1.data() );
    tmp += s2;
    return tmp;
}

Q_EXPORT inline QGString operator+( const char *s1, const QGString &s2 )
{
    QGString tmp( s1 );
    tmp += s2;
    return tmp;
}

Q_EXPORT inline QGString operator+( const QGString &s1, char c2 )
{
    QGString tmp( s1.data() );
    tmp += c2;
    return tmp;
}

Q_EXPORT inline QGString operator+( char c1, const QGString &s2 )
{
    QGString tmp;
    tmp += c1;
    tmp += s2;
    return tmp;
}

#endif // QGSTRING_H
