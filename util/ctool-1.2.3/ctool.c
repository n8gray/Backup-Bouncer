
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
**  File:    ctool.c
**  Date:    Feb 21, 2003
**
**  Author:  Brian Wotring
**  Purpose: perform MD5 or SHA1 pre-binding aware checksums on Mac OS X.
**
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/param.h>

#include <time.h>
#include <dirent.h>

#include <pwd.h>
#include <grp.h>

#include <mach/mach.h>
#include <mach-o/loader.h>

#include <openssl/md5.h>
#include <openssl/sha.h>

#include <curl/curl.h>

#include "ctool.h"
#include "safe_string.h"


int main( int argument_count, char *argument_list[] )
{
    int index;
    struct stat stats;

    parse_arguments( argument_count, argument_list );

    /* checksum each file/directory specified. */

    for( index = 1; index < argument_count; index++ )
    {
        /* skip options. */

        if( argument_list[index][0] == '-' )
        {
            /* skip over arguments to checksum and section flags. */

            if( ( argument_list[index][1] == 'a' ) &&
                ( (index + 1) < argument_count ) )
            {
                index++;
            }

            continue;
        }

        filename = argument_list[index];

        if( filename == NULL )
        {
            fprintf( stderr, "no file or directory specified.\n" );
            exit(0);
        }
  
        if( stat( filename, &stats ) != 0 )
        {
            fprintf( stderr, "unable to read file: (%s)\n", filename );
            exit(0);
        }

        if( S_ISDIR( stats.st_mode ) )
        {
            if( show_stat )
            {
                print_stats( filename, &stats );    
            }

            process_directory( filename );
        }
    
        else
        {
            process_file( filename );
        }
    }

    return 0;
}

/******************************************************************************
**
**    Function: parse_arguments
**
**    Purpose:  master routine to read in all of the CLAs and
**              take action appropriatly.
**
******************************************************************************/

void parse_arguments( int argument_count, char *argument_list[] )
{
    int count;

    if( argument_count == 1 )
    {
        print_usage();
        exit(0);
    }

    for( count = 1; count < argument_count; count++ )
    {
        /* handle these options right here. */

        if( ( strcmp( argument_list[count], "-h" ) == 0 ) ||
              strcmp( argument_list[count], "--help" ) == 0 )
        {
            print_usage();
            exit(0);
        }

        else if( ( strcmp( argument_list[count], "-v" ) == 0 ) ||
                   strcmp( argument_list[count], "--version" ) == 0 )
        {
            print_program_info();
            exit(0);
        }

        /* long options. */

        if( strncmp( argument_list[count], "--", 2 ) == 0 )
        {
            if( strcmp( argument_list[count], "--debug" ) == 0 )
            {
                debug = 1;
            }

            else if( strcmp( argument_list[count], "--recursive" ) == 0 )
            {
                recursive = 1;
            }

            else if( strcmp( argument_list[count], "--stat" ) == 0 )
            {
                show_stat = 1;
            }

            else if( strcmp( argument_list[count], "--knowngoods" ) == 0 )
            {
                knowngoods = 1;
            }
        }

        /* short options. */

        if( ( argument_list[count][0] == '-' ) && 
            ( strlen( argument_list[count] ) > 1 ) &&
            ( argument_list[count][1] != '-' ) )
        {
            int index;

            for( index = 1; index < strlen( argument_list[count] ); index++ )
            {
                if( argument_list[count][index] == 'd' )
                {
                    debug = 1;
                }

                else if( argument_list[count][index] == 'r' )
                {
                    recursive = 1;
                }

                else if( argument_list[count][index] == 's' )
                {
                    show_stat = 1;
                }

                else if( argument_list[count][index] == 'k' )
                {
                    knowngoods = 1;
                }
            }
        }

        if( ( strcmp( argument_list[count], "--alg" ) == 0 ||
              strcmp( argument_list[count], "-a" ) == 0 )
                      && ( count < ( argument_count - 1 ) ) )
        {
            count++;

            if( strncasecmp( argument_list[count], "sha", 3 ) == 0 )
            {
                use_md5 = 0;
            }

            else if( strncasecmp( argument_list[count], "md5", 3 ) == 0 )
            {
                use_md5 = 1;
            }

            else
            {
                fprintf( stderr, "unrecognized algorithm: (%s)\n", 
                         argument_list[count] );

                exit(-1);
            }
        }
    }
}

/******************************************************************************
**
**    Function: process_file
**
**    Purpose:  checksum specified file.
**
*******************************************************************************/

void process_file( const char *file_path )
{
    struct mach_header *mh;
    char path[MAXPATHLEN];

    struct load_command *load_commands;
    struct load_command *current_command;

    struct load_command command;
    struct stat stats;

    kern_return_t result;

    int fd;
    int length;

    unsigned long size;
    unsigned long index;
    unsigned long remaining;
    unsigned long file_pos;

    int burst;

    if( file_path == NULL )
    {
        return;
    }

    safe_strlcpy( path, file_path, sizeof( path ) );

    /* if we have a trailing slash, whack it. */

    length = safe_strnlen( path, sizeof( path ) );
    
    if( ( length ) && ( path[length-1] == '/' ) )
    {    
        path[length-1] = '\0';
    }

    if( ( fd = open( path, O_RDONLY ) ) == -1 )
    {
        fprintf( stderr, "unable to open file: %s\n", path );
        return;
    }

    /* attempt to stat the file. */

    if( fstat( fd, &stats ) == -1 )
    {
        fprintf( stderr, "unable to read file: %s\n", path );
        goto exit_gracefully;
    }

    size = stats.st_size;

    if( debug )
    {
        fprintf( stdout, "====[ file: (%s) ]====\n", path );
    }

    /* we can't process a zero length file. */

    if( size <= 0 )
    {
        fprintf( stderr, "...skipping zero length file.\n" );
        goto exit_gracefully;
    }

    if( ( result = map_fd( (int)fd, (vm_offset_t)0, (vm_offset_t *)&addr_base,
                           (boolean_t)TRUE, (vm_size_t)size) ) != KERN_SUCCESS )
    {
        fprintf( stderr, "unable to map file: %s\n", path );
        goto exit_gracefully;
    }

    initialize_hash_context();
    mh = (struct mach_header *)addr_base;

    if( debug )
    {
        print_file_type( mh->filetype );
    }

    if( mh->magic != MH_MAGIC )
    {
        has_prebinding = 0;

        if( debug )
        {
            fprintf( stderr, "file: (%s) is not an object file.\n", path );
        }

        /* normal hash, no pre-binding info here. */

        goto hash_file;
    }

    else if( ( mh->flags & MH_PREBOUND ) != MH_PREBOUND )
    {
        has_prebinding = 0;

        if( debug )
        {
            fprintf( stderr, "file: (%s) cannot be prebound.\n", path );
        }

        /* normal hash, no pre-binding info here. */

        goto hash_file;
    }

    else
    {
        has_prebinding = 1;
    }

    if( debug )
    {
        fprintf( stdout, "number of load commands: (%d)\n", mh->ncmds );
    }

    /* since this is a prebound file, we hash only certain */
    /* sections of the file.                               */

    checksum_section( "__TEXT", "__text", mh, fd );
    checksum_section( "__TEXT", "__cstring", mh, fd );
    checksum_section( "__TEXT", "__literal4", mh, fd );
    checksum_section( "__TEXT", "__literal8", mh, fd );

    checksum_section( "__DATA", "__mod_init_func", mh, fd );
    checksum_section( "__DATA", "__mod_term_func", mh, fd );

    goto print_hash;

hash_file:

    lseek( fd, 0, SEEK_SET );

    while(1)
    {
        burst = read( fd, hash_buffer, sizeof( hash_buffer ) );

        /* check error condition on read. */

        if( burst == 0 )
        {
            break;
        }

        if( burst < 0 )
        {
            fprintf( stderr, "error reading file.\n" );
            goto exit_gracefully;
        }

        update_hash_context( hash_buffer, (unsigned long)burst );
    }

    /* now we finalize the hash and print it. */

print_hash:

    if( debug )
    {
        int bytes_excluded = ( size - bytes_read );

        fprintf( stdout, "file size: (%d) bytes.\n", size );

        if( has_prebinding )
        {
            fprintf( stdout, "excluded a total of %d bytes from checksum.\n",
                     bytes_excluded );
        }
    }

    finalize_hash_context();
    print_hash_results( path );

    if( show_stat )
    {
        print_stats( path, &stats );
    }

    if( knowngoods )
    {
        verify_with_knowngoods( path, &stats );
    }

exit_gracefully:

    close( fd );
}

void checksum_section( const char *segment_name, const char *section_name,
                       struct mach_header *mh, int fd )
{
    int index;

    struct load_command *load_commands;
    struct load_command *current_command;
    struct load_command command;

    /* loop through the load commands, skipping prebinding info. */

    load_commands = (struct load_command *)(addr_base +
                                            sizeof(struct mach_header));

    current_command = load_commands;

    for( index = 0; index < mh->ncmds; index++ )
    {
        int num_sections;
        char *current_section;

        struct segment_command sc;
        struct section section;

        memcpy( (char *)&command, (char *)current_command,
                sizeof( struct load_command ) );

        if( command.cmd != LC_SEGMENT )
        {
            continue;
        }

        /* copy in the segment command structure. */

        memcpy( (char *)&sc, (char *)current_command,
                 sizeof( struct segment_command ) );

        num_sections = sc.nsects;

        /* not our desired segment. */

        if( strcmp( sc.segname, segment_name ) != 0 )
        {
            goto next_segment;
        }

        current_section = (char *)current_command +
                           sizeof(struct segment_command);

        while( num_sections > 0 )
        {
            int read_amount;

            memcpy( (char *)&section, current_section, sizeof(struct section) );
            current_section += sizeof( struct section );

            num_sections--;

            /* don't worry about empty sections. */
    
            if( section.size == 0 )
            {
                continue;
            }

            /* not our desired section. */

            if( ( strcmp( section.sectname, section_name ) != 0 ) ||
                ( section.offset == 0 ) )
            {
                continue;
            }

            read_amount = read_file_section( fd, section.offset, section.size );

            if( read_amount != section.size )
            {
                fprintf( stderr, "error reading file.\n" );
                return;
            }

            if( debug )
            {
             fprintf( stdout, "...read section %s,%s (offset=%lu,size=%lu).\n",
                         segment_name, section_name, section.offset,
                         section.size );
            }
        }

next_segment:

        /* catch some error conditions in the load commands. */

        if( command.cmdsize == 0 )
        {
            if( debug )
            {
                fprintf( stderr, "load command is zero in size, stopping.\n" );
            }

            return;
        }

        /* move to next load command. */

        current_command = (struct load_command *)((char *)current_command +
                                                          command.cmdsize );

        if( (char *)current_command > (char *)load_commands + mh->sizeofcmds )
        {
            if( debug )
            {
                fprintf( stderr,
                         "load command is larger than command section.\n" );

            }
        
            return;    
        }

    } /* end loop through load commands. */
}

int read_file_section( int fd, int offset, int amount )
{
    int burst = 0;
    int left  = amount;

    /* fist skip to the offset. */

    if( lseek( fd, offset, SEEK_SET ) != offset )
    {
        fprintf( stderr, "unable to seek file!\n" );
        return -1;
    }

    while( left > 0 )
    {
        int to_read = ( left > sizeof( hash_buffer ) ) ?
                        sizeof( hash_buffer ) : left;

        burst = read( fd, hash_buffer, to_read );

        if( burst <= 0 )
        {
            fprintf( stderr, "error reading file.\n" );
            return -1;
        }

        update_hash_context( hash_buffer, (unsigned long)burst );
        left = ( left - burst );
    }

    return amount;
}

char * get_name_for_load_command( int command )
{
    switch( command )
    {
        case LC_SEGMENT:
            return "LC_SEGMENT";

        case LC_SYMTAB:
            return "LC_SYMTAB";

        case LC_SYMSEG:
            return "LC_SYMSEG";

        case LC_THREAD:
            return "LC_THREAD";

        case LC_UNIXTHREAD:
            return "LC_UNIXTHREAD";

        case LC_LOADFVMLIB:
            return "LC_LOADFVMLIB";

        case LC_IDFVMLIB:
            return "LC_IDFVMLIB";

        case LC_IDENT:
            return "LC_IDENT";

        case LC_FVMFILE:
            return "LC_FVMFILE";

        case LC_PREPAGE:
            return "LC_PREPAGE";

        case LC_DYSYMTAB:
            return "LC_DYSYMTAB";

        case LC_LOAD_DYLIB:
            return "LC_LOAD_DYLIB";

        case LC_ID_DYLIB:
            return "LC_ID_DYLIB";

        case LC_LOAD_DYLINKER:
            return "LC_LOAD_DYLINKER";

        case LC_ID_DYLINKER:
            return "LC_ID_DYLINKER";

        case LC_PREBOUND_DYLIB:
            return "LC_PREBOUND_DYLIB";

        case LC_ROUTINES:
            return "LC_ROUTINES";

        case LC_SUB_FRAMEWORK:
            return "LC_SUB_FRAMEWORK";

        case LC_SUB_UMBRELLA:
            return "LC_SUB_UMBRELLA";

        case LC_SUB_CLIENT:
            return "LC_SUB_CLIENT";

        case LC_SUB_LIBRARY:
            return "LC_SUB_LIBRARY";

        case LC_TWOLEVEL_HINTS:
            return "LC_TWOLEVEL_HINTS";

        case LC_PREBIND_CKSUM:
            return "LC_PREBIND_CKSUM";

        case LC_LOAD_WEAK_DYLIB:
            return "LC_LOAD_WEAK_DYLIB";

        default:

            return ( "unknown" );
            break;
    }
}


void print_stats( const char *file_path, struct stat *stats )
{
    if( ( file_path != NULL ) && ( stats != NULL ) )
    {
        char buffer[15];
        char time_buffer[30];

        char name[100];
        char group[100];
        struct passwd *pass;
        struct group *grp;

        fprintf( stdout, "\nstats for (%s):\n\n", file_path );
        fprintf( stdout, "      device: %lu\n", stats->st_dev );
        fprintf( stdout, "       inode: %lu\n", stats->st_ino );

        /* file permissisons string.*/

        get_file_attribute_string( buffer, sizeof(buffer), stats->st_mode );
        fprintf( stdout, "        mode: %s (%lu)\n", buffer, stats->st_mode );
        fprintf( stdout, "       links: %lu\n", stats->st_nlink );

        /* display user and group names. */

        name[0] = '\0';
        group[0] = '\0';

        pass = getpwuid( stats->st_uid );
        grp = getgrgid( stats->st_gid );

        if( pass != NULL )
        {
            safe_strlcpy( name, pass->pw_name, sizeof( name ) );
        }

        if( grp != NULL )
        {
            safe_strlcpy( group, grp->gr_name, sizeof( group ) );
        }

        fprintf( stdout, "         uid: %lu %s\n", stats->st_uid, name );
        fprintf( stdout, "         gid: %lu %s\n", stats->st_gid, group );

        fprintf( stdout, "        rdev: %lu\n", stats->st_rdev );

       /* mtime. */

        safe_strlcpy( time_buffer, ctime( &stats->st_mtime ),
                      sizeof( time_buffer ) );
        time_buffer[24] = '\0';

        fprintf( stdout, "       mtime: %s (%lu)\n", time_buffer,
                 stats->st_mtime );

        /* atime. */

        safe_strlcpy( time_buffer, ctime( &stats->st_atime ),
                      sizeof( time_buffer ) );
        time_buffer[24] = '\0';

        fprintf( stdout, "       atime: %s (%lu)\n", time_buffer,
                 stats->st_atime );

        /* ctime */

        safe_strlcpy( time_buffer, ctime( &stats->st_ctime ),
                      sizeof( time_buffer ) );
        time_buffer[24] = '\0';

        fprintf( stdout, "       ctime: %s (%lu)\n", time_buffer,
                 stats->st_ctime );

        fprintf( stdout, "       bytes: %llu\n", stats->st_size );
        fprintf( stdout, "      blocks: %llu\n", stats->st_blocks );
        fprintf( stdout, "  block size: %lu\n", stats->st_blksize );
        fprintf( stdout, "       flags: %lu\n", stats->st_flags );
        fprintf( stdout, "  gen number: %lu\n", stats->st_gen );

        fprintf( stdout, "\n" );
    }
}

void verify_with_knowngoods( const char *file_path, struct stat *stats )
{
    CURL *curl;
    CURLcode response;

    if( ( file_path == NULL ) || ( stats == NULL ) )
    {
        return;
    }

    curl = curl_easy_init();

    if( curl )
    {
        char kg_get_url[4096] = KG_GET_URL;
        fprintf( stdout,
                "==> verifying checksum with https://knowngoods.org...\n" );

        /* append our hash value and ask knowngoods.org for it. */

        safe_strlcat( kg_get_url, file_hash, sizeof( kg_get_url ) );

        curl_easy_setopt( curl, CURLOPT_URL, kg_get_url );
        curl_easy_setopt( curl, CURLOPT_NOPROGRESS );

        response = curl_easy_perform( curl );
        curl_easy_cleanup( curl );

        fprintf( stdout, "\n" );
    }
}

void print_file_type( unsigned long type )
{
    fprintf( stdout, "%s: ", "file type" );

    switch( (int)type )
    {
        case MH_OBJECT:
            fprintf( stdout, "%s", "intermediate object file" );
            break;

        case MH_EXECUTE:
            fprintf( stdout, "%s", "executable" );
            break;

        case MH_BUNDLE:
            fprintf( stdout, "%s", "bundle/plug-in" );
            break;

        case MH_DYLIB:
            fprintf( stdout, "%s", "dynamic shared library" );
            break;

        case MH_PRELOAD:
            fprintf( stdout, "%s", "special purpose executable" );
            break;

        case MH_CORE:
            fprintf( stdout, "%s", "core file" );
            break;

        case MH_DYLINKER:
            fprintf( stdout, "%s", "dynamic linker shared library" );
            break;

        default:
            fprintf( stdout, "%s", "unknown" );
            break;
    }

    fprintf( stdout, "%s", "\n" );
}

void print_hash_results( const char *file_path )
{
    int index;

    /* print the MD5 or SHA1 value to standard output. */

    if( use_md5 )
    {
        for( index = 0; index < MD5_DIGEST_LENGTH; index++ )
        {
            safe_snprintf( &file_hash[index * 2], 3, "%02x",
                           md5_digest[index] );
        }

        file_hash[32] = '\0';

        if( has_prebinding )
        {
            fprintf( stdout, "ctool-" );
        }

        fprintf( stdout, "MD5(%s)= %s\n", file_path, file_hash );
    }

    else
    {
        for ( index = 0; index < SHA_DIGEST_LENGTH; index++)
        {
            safe_snprintf( &file_hash[index * 2], 3, "%02x",
                           sha_digest[index] );
        }

        file_hash[40] = '\0';

        if( has_prebinding )
        {
            fprintf( stdout, "ctool-" );
        }

        fprintf( stdout, "SHA1(%s)= %s\n", file_path, file_hash );
    }
}

void initialize_hash_context()
{
    bytes_read = 0;

    if( use_md5 )
    {
        MD5_Init( &md5_context );
    }

    else
    {
        SHA1_Init( &sha_context );
    }
}

void update_hash_context( unsigned char *data, unsigned long data_length )
{
    if( ( data == NULL ) || ( data_length == 0 ) )
    {
        return;
    }

    bytes_read += data_length;

    if( use_md5 )
    {
        MD5_Update( &md5_context, data, data_length );
    }

    else
    {
        SHA1_Update( &sha_context, data, data_length );
    }
}

void finalize_hash_context()
{
    if( use_md5 )
    {
        MD5_Final( &( md5_digest[0] ), &md5_context );
    }

    else
    {
        SHA1_Final( &( sha_digest[0] ), &sha_context );
    }
}

/******************************************************************************
**
**    Function: process_directory
**
**    Purpose:  checksum each file in the directory specified.
**
*******************************************************************************/

void process_directory( const char *file_path )
{
    int length;
    DIR *directory;

    char dir_path[MAXPATHLEN];
    char path[MAXPATHLEN];

    struct dirent *file;
    struct stat stats;

    if( file_path == NULL )
    {
        return;
    }

    safe_strlcpy( dir_path, file_path, sizeof( dir_path ) );

    /* if we have a trailing slash, whack it. */

    length = safe_strnlen( dir_path, sizeof( dir_path ) );

    if( ( length ) && ( dir_path[length-1] == '/' ) )
    {
        dir_path[length-1] = '\0';
    }

    if( ( directory = opendir( dir_path ) ) == NULL )
    {
        fprintf( stderr, "error: unable to open directory (%s)\n", dir_path );
        return;
    }

    while( ( file = readdir( directory ) ) != NULL )
    {
        /* skip current and parent directory. */

        if( ( strcmp( file->d_name, "." ) == 0 ) ||
            ( strcmp( file->d_name, ".." ) == 0 ) )
        {
            continue;
        }

        safe_strlcpy( path, dir_path, sizeof( path ) );
        safe_strlcat( path, "/", sizeof( path ) );
        safe_strlcat( path, file->d_name, sizeof( path ) );

        if( file->d_type == DT_DIR )
        {
            if( recursive )
            {
                process_directory( path );
            }
        }        

        else
        {
            process_file( path );
        }
    }

    closedir( directory );
}

/******************************************************************************
**
**    Function: print_usage
**
**    Purpose:  prints usage statement and all command line arguments
**
*******************************************************************************/

void print_usage()
{

    fprintf( stdout,"\
\n\
usage: ctool [options...] <file> | <directory> ...\n\
\n\
  options:\n\
\n\
     -a, --alg <alg>   one of {md5,sha1} default is md5.\n\
     -d, --debug       verbose; display verbose debugging information.\n\
     -r, --recursive   recursively process a directory.\n\
     -s, --stat        show all stat information for specified file.\n\
     -k, --knowngoods  verify the resultant checksum(s) with knowngoods.org.\n\
\n\
     -v, --version     display program version information.\n\
     -h, --help        display this usage statement.\n\n" );
}

/******************************************************************************
**
**    Function: void print_program_info
**
**    Purpose:  prints program name, version, authors, etc.
**
******************************************************************************/

void print_program_info()
{
    fprintf( stdout, "%s version %s - The Shmoo Group (c) 2003\n",
                         PROGRAM_NAME, PROGRAM_VERSION );
}


/******************************************************************************
**
**    Function: get_file_attribute_string
**
**    Purpose:  get the permissions string for the file in the
**              standard 10 char form -rwxrwxrwx
**
******************************************************************************/

void get_file_attribute_string( char *buffer, int buffer_size,
                                mode_t attributes )
{
    if( ( buffer != NULL ) && ( buffer_size >= 11 ) )
    {
        memset( buffer, '-', buffer_size );

        /* first, determine type. */

        if( S_ISREG( attributes ) )
        {
            buffer[0] = '-';
        }

        else if( S_ISCHR( attributes ) )
        {
            buffer[0] = 'c';
        }

        else if( S_ISDIR( attributes ) )
        {
            buffer[0] = 'd';
        }

        else if( S_ISBLK( attributes ) )
        {
            buffer[0] = 'b';
        }

        else if( S_ISLNK( attributes ) )
        {
            buffer[0] = 'l';
        }

        else if( S_ISFIFO( attributes ) )
        {
            buffer[0] = 'p';
        }

        else if( S_ISSOCK( attributes ) )
        {
            buffer[0] = 's';
        }

        else
        {
            buffer[0] = '?';
        }

        /* set fields for user/group/other bits in the string. */

        if ( attributes & S_IRUSR )
        {
            buffer[1] = 'r';
        }

        if ( attributes & S_IWUSR )
        {
            buffer[2] = 'w';
        }

        if( attributes & S_IXUSR )
        {
            if ( attributes & S_ISUID )
            {
                buffer[3] = 's';
            }

            else
            {
                buffer[3] = 'x';
            }
        }

        else if( attributes & S_ISUID )
        {
            buffer[3] = 'S';
        }

        /* set the group bit fields. */

        if ( attributes & S_IRGRP )
        {
            buffer[4] = 'r';
        }

        if ( attributes & S_IWGRP )
        {
            buffer[5] = 'w';
        }

        if( attributes & S_IXUSR )
        {
            if ( attributes & S_ISGID )
            {
                buffer[6] = 's';
            }

            else
            {
                buffer[6] = 'x';
            }
        }

        else if( attributes & S_ISGID )
        {
            buffer[6] = 'S';
        }


        /* set the other bit fields. */

        if( attributes & S_IROTH )
        {
            buffer[7] = 'r';
        }

        if( attributes & S_IWOTH )
        {
            buffer[8] = 'w';
        }

#ifdef S_ISVTX

        if( attributes & S_IXOTH )
        {
            if( attributes & S_ISVTX )
            {
                buffer[9] = 't';
            }

            else
            {
                buffer[9] = 'x';
            }
        }

        else if( attributes & S_ISVTX )
        {
            buffer[9] = 'T';
        }

#else
        if( attributes & S_IXOTH )
        {
            buffer[9] = 'x';
        }
#endif
        buffer[10] = '\0';
    }
}

