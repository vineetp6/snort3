/*
** Copyright (C) 2014-2015 Cisco and/or its affiliates. All rights reserved.
** Copyright (C) 2003-2013 Sourcefire, Inc.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License Version 2 as
** published by the Free Software Foundation.  You may not use, modify or
** distribute this program under any other version of the GNU General
** Public License.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
 
/*
*
*  sfghash.h
*
*  generic hash table - stores and maps key + data pairs
*
*  Author: Marc Norton
*
*/

#ifndef SFGHASH_H
#define SFGHASH_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

struct SFHASHFCN;

/*
*   ERROR DEFINES
*/
#define SFGHASH_NOMEM    -2
#define SFGHASH_ERR      -1
#define SFGHASH_OK        0
#define SFGHASH_INTABLE   1

/* 
*  Flags for ghash_new: userkeys 
*/
#define GH_COPYKEYS 0
#define GH_USERKEYS 1

/*
*   Generic HASH NODE
*/
struct SFGHASH_NODE
{
  struct SFGHASH_NODE * next, * prev;

  void * key;   /* Copy of, or Pointer to, the Users key */
  void * data;  /* Pointer to the users data, this is never copied! */
     
} ;

/*
*    Generic HASH table
*/
struct SFGHASH
{
  SFHASHFCN    * sfhashfcn;
  int          keysize;   /* bytes in key, if < 0 -> keys are strings */
  int          userkey;   /* user owns the key */

  SFGHASH_NODE ** table;  /* array of node ptr's */
  int             nrows;  /* # rows int the hash table use a prime number 211, 9871 */

  unsigned       count;  /* total # nodes in table */

  void         (*userfree)( void * );  

  int            crow;    // findfirst/next row in table
  SFGHASH_NODE * cnode; // findfirst/next node ptr

  int splay;

};


/*
*   HASH PROTOTYPES
*/
SFGHASH * sfghash_new( int nrows, int keysize, int userkeys, void (*userfree)(void*p) );
void      sfghash_delete( SFGHASH * h );
int       sfghash_add ( SFGHASH * h, void * key, void * data );
int       sfghash_remove( SFGHASH * h, void * key);
int       sfghash_count( SFGHASH * h);
void    * sfghash_find( SFGHASH * h, const void * key );
int       sfghash_find2(SFGHASH *, const void *, void **);
SFGHASH_NODE * sfghash_findfirst( SFGHASH * h );
SFGHASH_NODE * sfghash_findnext ( SFGHASH * h );
void sfghash_splaymode( SFGHASH * t, int n );

int sfghash_set_keyops( SFGHASH *h ,
                        unsigned (*hash_fcn)( SFHASHFCN * p,
                                              unsigned char *d,
                                              int n),
                        int (*keycmp_fcn)( const void *s1,
                                           const void *s2,
                                           size_t n));


#endif

