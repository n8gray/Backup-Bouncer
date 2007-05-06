
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
**  File:    safe_string.h
**  Date:    June 22, 2001
**
**  Purpose: somewhat safe C string operations.
**
******************************************************************************/

#ifndef SAFE_STRING_H_
#define SAFE_STRING_H_

#define MINIMUM( first, second )    ( ( first > second ) ? second : first )

size_t safe_strnlen( const char *str, size_t nchars );

size_t osi_strlcpy( char *dst, const char *src, size_t nchars );
size_t osi_strlcat( char *dst, const char *src, size_t nchars );

size_t osi_vsnprintf( char *str, size_t nchars, const char  *format,
                      va_list ap );

size_t osi_snprintf( char *str, size_t nchars, const char *format, ... );

#endif
