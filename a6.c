#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"


//steps 1 & 2
//get index of value in filename
//returns the index received from hash_lookup
long getIndex(char* filename, char* value) {
  
  //initialize
  char idxname[BUFFER];
  char txtname[BUFFER];
  long hash_table[ HASHSIZE ];

  // identify text file name
  strcpy( idxname, filename );
  strcat( idxname, ".idx" );
  strcpy( txtname, filename );
  strcat( txtname, ".txt" );

  // load hashtable from file into memory
  get_hashtable( filename, hash_table );

  // open text file
  FILE *idxfile = fopen( idxname, "r" );
  FILE *txtfile = fopen( txtname, "r" );

  // get result of hash_lookup
  long index = hash_lookup( value, hash_table, idxfile, txtfile );
  
  //close the file
  fclose( idxfile );
  fclose( txtfile );

  //return the value
  return index;

}

int main( int argc, char **argv ) {
  
  //value of indicies after hash_lookup()
  long indexBuilding;
  long indexRoom;

  //the building and room value given by argv[1] & argv[2]
  char *buildingValue = argv[1];
  char *roomValue = argv[2];

  //make sure user inputs the correct command line (+ arguments)
  if (argc!=3) {
    fprintf( stderr, "Usage: %s <buildingName> <roomNumber>\n", argv[0] );
    exit(-1);
  }

  // ---------- GET INDEX OF BUILDING ---------- //

  indexBuilding = getIndex("building", buildingValue);
  printf( "Index of building %s: %ld\n",buildingValue, indexBuilding );

  // ---------- GET INDEX OF BUILDING ---------- //

  // ---------- GET INDEX OF ROOM ---------- //

  indexRoom = getIndex("room", roomValue);
  printf( "Index of room %s: %ld\n", roomValue, indexRoom );

  // ---------- GET INDEX OF ROOM ---------- //




//   printf( “%s*%s %s %s - %s\n”, subject, courseno, days, from, to );


  return 0;
} 
