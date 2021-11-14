#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

int main( int argc, char **argv ) {
  
  //filenames
  char idxname[BUFFER];
  char txtname[BUFFER];
  
  //value of indicies after hash_lookup()
  long indexBuilding;
  long indexRoom;

  //file to be opened
  char *basename1 = "building";
  char *basename2 = "room";

  //the building and room value given by argv[1] & argv[2]
  char *buildingValue;
  char *roomValue;

  FILE *idxfile;
  FILE *txtfile;

  long hash_table[ HASHSIZE ];

  //make sure user inputs the correct command line (+ arguments)
  if (argc!=3) {
    fprintf( stderr, "Usage: %s <buildingName> <roomNumber>\n", argv[0] );
    exit(-1);
  }

  // target values
  buildingValue = argv[1];
  roomValue = argv[2];

  // ---------- GET INDEX OF BUILDING ---------- //

  // identify text file name
  strcpy( idxname, basename1 );
  strcat( idxname, ".idx" );
  strcpy( txtname, basename1 );
  strcat( txtname, ".txt" );


  // load hashtable from file into memory
  get_hashtable( basename1, hash_table );

  // open text file
  idxfile = fopen( idxname, "r" );
  txtfile = fopen( txtname, "r" );

  // get result of hash_lookup
  indexBuilding = hash_lookup( buildingValue, hash_table, idxfile, txtfile );
  printf( "Index of building %s: %ld\n",buildingValue, indexBuilding );

  //close the file
  fclose( idxfile );
  fclose( txtfile );

  // ---------- GET INDEX OF BUILDING ---------- //

  // ---------- GET INDEX OF ROOM ---------- //

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

  // get result of hash_lookup
  indexRoom = hash_lookup( roomValue, hash_table, idxfile, txtfile );
  printf( "Index of room %s: %ld\n", roomValue, indexRoom );

  fclose( idxfile );
  fclose( txtfile );

  // ---------- GET INDEX OF ROOM ---------- //




//   printf( “%s*%s %s %s - %s\n”, subject, courseno, days, from, to );


  return 0;
} 
