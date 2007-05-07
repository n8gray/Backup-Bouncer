
/******************************************************************************
**
**  This program is free software; you can redistribute it and/or
**  modify it, however, you cannot sell it.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
**
**  You should have received a copy of the license attached to the
**  use of this software.  If not, visit www.shmoo.com/osiris for
**  details.
**
******************************************************************************/

/*****************************************************************************
**
**  File:    safe_string.c
**  Date:    June 22, 2001
**
**  Purpose: somewhat safe C string operations.
**
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "safe_string.h"



size_t safe_strnlen( const char *str, size_t nchars )
{
    size_t i = 0;
    size_t result = 0;

    if( str && nchars > 0 )
    {
        for( i = 0; i < nchars && str[i] != '\0'; i++ )
        {
            result++;
        }
    }

    return result;
}

size_t safe_strlcpy( char * dst, const char *src, size_t nchars )
{
    size_t cpy_len;
    size_t src_len = 0;

    if( src == NULL )
    {
        goto exit_gracefully;
    }

    // while nchars doesn't strictly apply to src, we're not going to
    // be able to copy more than that many characters from it anyway...

    src_len = safe_strnlen( src, nchars );

    if( dst == NULL )
    {
        goto exit_gracefully;
    }

    if( nchars > 0 )
    {
        cpy_len = MINIMUM( src_len, ( nchars - 1 ) );
        memcpy( dst, src, cpy_len );

        dst[cpy_len] = '\0';
    }

exit_gracefully:

    //note: this is src because we're returning how much
    //we would have written, not how much we did write.

    return src_len;
}

size_t safe_strlcat( char *dst, const char *src, size_t nchars )
{
    size_t orig_dst_len = 0;
    size_t src_len = 0;
    size_t result = nchars;
    size_t space_left, cpy_len;
    char * cpy_start;

    if( !dst )
    {
        goto exit_gracefully;
    }

    //from the openbsd strlcat manpage:
    // Note however, that if strlcat() traverses size characters without finding
    // a NUL, the length of the string is considered to be size and the destina-
    // tion string will not be NUL-terminated (since there was no space for the
    // NUL).  This keeps strlcat() from running off the end of a string.  In
    // practice this should not happen (as it means that either size is incor-
    // rect or that dst is not a proper ``C'' string).  The check exists to pre-
    // vent potential security problems in incorrect code.

    orig_dst_len = safe_strnlen( dst, nchars );

    if( !src )
    {
        goto exit_gracefully;
    }

    // while nchars doesn't strictly apply to src, we're not going to
    // be able to copy more than that many characters from it anyway...

    src_len = safe_strnlen( src,nchars );
    space_left = nchars - orig_dst_len;

    if( space_left > 0 )
    {
        cpy_start = dst + orig_dst_len;
        cpy_len = MINIMUM( src_len, ( space_left - 1 ) );
        memcpy( cpy_start, src, cpy_len );
        cpy_start[cpy_len] = '\0';

        //remember, how big the write *would* have been...

        result = orig_dst_len + src_len;
    }

exit_gracefully:
    return result;
}

size_t safe_vsnprintf( char *str, size_t nchars,
                      const char * format, va_list ap )
{
    size_t result = nchars;

    if( nchars < 1 || !str || !format )
    {
        goto exit_gracefully;
    }

#ifdef _WIN32
    result = _vsnprintf( str, nchars, format, ap );
#else
    result = vsnprintf( str, nchars, format, ap);
#endif /* _WIN32 */

    //some (v)snprintf implmentations don't write a NUL at the end
    //of the buffer if it is overrun. So, ensure that the last char
    //in the buffer is NUL so that we can check if it was overwritten
    //by snprintf.

    str[nchars - 1] = '\0';

    //some (v)snprintf implementations return -1 on overrun. instead,
    //return act like the C99 standard: overrun means
    //return val is greater than or equal to the buffersize. However,
    //C99 says snprintf should return how much buffer space *would*
    //have been needed. We can't easily calculate that if the underlying
    //snprintf doesn't do it for us.

    if( result < 0 )
    {
        result = nchars;
    }

exit_gracefully:
    return result;
}

size_t safe_snprintf( char *str, size_t nchars, const char *format, ... )
{
    size_t result;
    va_list args;

    va_start( args, format );
    result = safe_vsnprintf( str, nchars, format, args );
    va_end( args );

    return result;
}
