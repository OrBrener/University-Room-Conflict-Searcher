#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

int main( int argc, char **argv ) {
  char idxname[BUFFER];
  char txtname[BUFFER];

  char *basename1 = "building";
  char *basename2 = "room";
  char *buildingValue;
  char *roomValue;
  long hash_table[ HASHSIZE ];

  if (argc!=3)
  {
    fprintf( stderr, "Usage: %s basename value\n", argv[0] );
    exit(-1);
  }

  // target value
  buildingValue = argv[1];
  roomValue = argv[2];

  // identify text file name
  strcpy( idxname, basename1 );
  strcat( idxname, ".idx" );
  strcpy( txtname, basename1 );
  strcat( txtname, ".txt" );


  // load hashtable from file into memory
  get_hashtable( basename1, hash_table );

  // open text file
  FILE *idxfile = fopen( idxname, "r" );
  FILE *txtfile = fopen( txtname, "r" );

  // print result of hash_lookup
  printf( "%ld\n", hash_lookup( buildingValue, hash_table, idxfile, txtfile ) );

  fclose( idxfile );
  fclose( txtfile );

  // identify text file name
  strcpy( idxname, basename2 );
  strcat( idxname, ".idx" );
  strcpy( txtname, basename2 );
  strcat( txtname, ".txt" );


  // load hashtable from file into memory
  get_hashtable( basename2, hash_table );

  // open text file
  idxfile = fopen( idxname, "r" );
  txtfile = fopen( txtname, "r" );

  // print result of hash_lookup
  printf( "%ld\n", hash_lookup( roomValue, hash_table, idxfile, txtfile ) );

  fclose( idxfile );
  fclose( txtfile );




//   printf( “%s*%s %s %s - %s\n”, subject, courseno, days, from, to );


  return 0;
} 
