
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
**  File:    ctool.h
**  Date:    Feb 21, 2003
**
**  Author:  Brian Wotring
**  Purpose: perform MD5 or SHA1 pre-binding aware checksums on Mac OS X.
**
******************************************************************************/

#ifndef CTOOL_H_
#define CTOOL_H_

#define PROGRAM_NAME      "ctool"
#define PROGRAM_VERSION   "1.2.3"

#define MINIMUM( first, second )    ( ( first > second ) ? second : first )
#define KG_GET_URL "https://knowngoods.org/search.php?locate="


char *addr_base;
const char *filename = NULL;

int use_md5    = 1;
int debug      = 0;
int recursive  = 0;
int show_stat  = 0;
int knowngoods = 0;

int has_prebinding = 0;

int total_size     = 0;
int bytes_read     = 0;

MD5_CTX md5_context;
SHA_CTX sha_context;

unsigned char md5_digest[MD5_DIGEST_LENGTH+1];
unsigned char sha_digest[SHA_DIGEST_LENGTH+1];

unsigned char file_hash[41];
unsigned char hash_buffer[512];


void parse_arguments( int argument_count, char **argument_list );

void process_file( const char *file_path );
void process_directory( const char *file_path );

void print_usage();
void print_program_info();
void print_file_type( unsigned long type );
void print_hash_results( const char *file_path );
void print_stats( const char *file_path, struct stat *stats );
void verify_with_knowngoods( const char *file_path, struct stat *stats );

void initialize_hash_context();
void update_hash_context( unsigned char *data, unsigned long data_length );
void finalize_hash_context();

void get_file_attribute_string( char *buffer, int buffer_size,
                                mode_t attributes );

char * get_name_for_load_command( int command );

void checksum_section( const char *segment_name, const char *section_name,
                       struct mach_header *mh, int fd );

int read_file_section( int fd, int offset, int amount );

#endif
