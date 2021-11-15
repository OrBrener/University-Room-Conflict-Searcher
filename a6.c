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

//function taken from query.c
int entries( char *filebase ) {
  char filename[BUFFER];
  int ent;

  strcpy( filename, filebase );
  strcat( filename, ".idx" );
  FILE *fp = fopen( filename, "rb" );
  fseek( fp, 0, SEEK_END );
  ent = ftell( fp ) / sizeof(long);
  fclose(fp);

  return ent;
}

//steps 3 & 4
//creates an output set file that has all the course codes based on the input file and searchValue/index
void getQuery(int searchValue, char* inputFile, int index, char* outputFile) {
  char *v1 = "code";
  int i1 = searchValue;
  char *v2 = inputFile;
  int i2 = index;

  int n1 = entries( v1 );
  int n2 = entries( v2 );


  char filename[BUFFER];

  strcpy( filename, v1 );
  strcat( filename, "_" );
  strcat( filename, v2 );
  strcat( filename, ".rel" );

  FILE *fp = fopen( filename, "rb" );
  char *array = malloc( n1*n2 );
  fread( array, 1, n1*n2, fp );
  fclose( fp );

  fp = fopen( outputFile, "wb" );
  if ( (i1==-1) && (i2>=0) )
  {
    for (int i=0;i<n1;i++)
    {
      int index = i*n2 +i2;
      fwrite( array+index, 1, 1, fp );
    }
  }

  if ( (i1>0) && (i2==-1) )
  {
    for (int j=0;j<n2;j++)
    {
      int index = i1*n2 + j;
      fwrite( array+index, 1, 1, fp );
    }
  }

  fclose( fp );
}

//step 5
//create an output set file that is the intersection of setFile1 and setFile2
void and(char* setFile1, char* setFile2, char* outputFile) {
  FILE *fp1 = fopen( setFile1, "rb" );
  FILE *fp2 = fopen( setFile2, "rb" );
  FILE *fp3 = fopen( outputFile, "wb" );
  char b1, b2, b3;

  while ( fread( &b1, 1, 1, fp1 )==1 && fread( &b2, 1, 1, fp2 ) )
  {
    b3 = b1&&b2;
    fwrite( &b3, 1, 1, fp3 );
  }

  fclose( fp1 );
  fclose( fp2 );
  fclose( fp3 );
}

int set2Index_oneValue(char* filename) {
  char boolean;

  FILE *fp = fopen( filename, "rb" );

  for (int i=0; fread(&boolean,1,1,fp)==1; i++)
  {
    if (boolean)
      return i;
  }

  return -1;
}

char* getString(char* basenameInput, int index) {
  char *basename;
  int idx, idx2;

  basename = basenameInput;
  idx = index;

  char txtfile[BUFFER];
  char idxfile[BUFFER];
  char* buffer = malloc (BUFFER);

  FILE *fptxt, *fpidx;

  strcpy( txtfile, basename );
  strcat( txtfile, ".txt" );

  strcpy( idxfile, basename );
  strcat( idxfile, ".idx" );

  fptxt = fopen( txtfile, "r" );
  fpidx = fopen( idxfile, "r" );

  fseek( fpidx, sizeof(long)*idx, SEEK_SET );
  
  if ( fread( &idx2, sizeof(long), 1, fpidx ) != 1 )
  {
    fprintf( stderr, "Error: invalid index\n" );
    exit(-1);
  }

  fclose( fpidx );

  fseek( fptxt, idx2, SEEK_SET );
  fgets( buffer, BUFFER, fptxt );
  fclose( fptxt );

  buffer[strlen(buffer)-1 ] = '\0';

  return buffer;
}

int main( int argc, char **argv ) {
  
  //value of indicies after hash_lookup()
  long indexBuilding;
  long indexRoom;

  //the building and room value given by argv[1] & argv[2]
  char* buildingValue = argv[1];
  char* roomValue = argv[2];

  char* buildingFile = "building.set";
  char* roomFile = "file.set";
  char* intersectionFile = "intersection.set";


  //make sure user inputs the correct command line (+ arguments)
  if (argc!=3) {
    fprintf( stderr, "Usage: %s <buildingName> <roomNumber>\n", argv[0] );
    exit(-1);
  }

  // ---------- GET INDEX OF BUILDING ---------- //

  indexBuilding = getIndex("building", buildingValue);
  // printf( "Index of building %s: %ld\n",buildingValue, indexBuilding );

  // ---------- GET INDEX OF BUILDING ---------- //

  // ---------- GET INDEX OF ROOM ---------- //

  indexRoom = getIndex("room", roomValue);
  // printf( "Index of room %s: %ld\n", roomValue, indexRoom );

  // ---------- GET INDEX OF ROOM ---------- //

  // ---------- SET FILE OF INDICES OF ALL COURSES IN BUILDING_INDEX ---------- //

  getQuery(-1, "building", indexBuilding, buildingFile); 

  // ---------- SET FILE OF INDICES OF ALL COURSES IN BUILDING_INDEX ---------- //

  // ---------- SET FILE OF INDICES OF ALL COURSES IN ROOM_INDEX ---------- //

  getQuery(-1, "room", indexRoom, roomFile); 

  // ---------- SET FILE OF INDICES OF ALL COURSES IN ROOM_INDEX ---------- //

  // ---------- SET FILE OF INDICES OF INTERSECTION OF BUILDING_FILE AND ROOM_FILE ---------- //
  
  and(buildingFile, roomFile, intersectionFile);

  // ---------- SET FILE OF INDICES OF INTERSECTION OF BUILDING_FILE AND ROOM_FILE ---------- //

  char boolean;

  FILE *fp = fopen( intersectionFile, "rb" );

  for (int i=0; fread(&boolean,1,1,fp)==1; i++)
  {
    if (boolean) {
      getQuery(i, "subject", -1, "subject.set");
      int subjectIndex = set2Index_oneValue("subject.set");
      char* subject = getString("subject", subjectIndex);
      // printf("SUBJECT: %s\n", subject);

      getQuery(i, "courseno", -1, "courseno.set");
      int courseNumberIndex = set2Index_oneValue("courseno.set");
      char* courseno = getString("courseno", courseNumberIndex);
      // printf("COURSENO: %s\n", courseno);

      getQuery(i, "days", -1, "days.set");
      int daysIndex = set2Index_oneValue("days.set");
      char* days = getString("days", daysIndex);
      // printf("DAYS: %s\n", days);

      getQuery(i, "from", -1, "from.set");
      int fromIndex = set2Index_oneValue("from.set");
      char* from = getString("from", fromIndex);
      // printf("FROM: %s\n", from);

      getQuery(i, "to", -1, "to.set");
      int toIndex = set2Index_oneValue("to.set");
      char* to = getString("to", toIndex);
      // printf("TO: %s\n", to);

      printf( "%s*%s %s %s - %s\n", subject, courseno, days, from, to );

      free(subject);
      free(courseno);
      free(days);
      free(from);
      free(to);
    }
  }



  return 0;
} 
