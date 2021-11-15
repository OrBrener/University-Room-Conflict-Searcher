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

//return the index from a file 
//only for one line
//used to read the subject/courseno/days/to/from values of each line in the intersection
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


//outputs the string value of the basename at index 
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

//given command line arguments <buildingName> <roomNumber>
//outputs all the courses that take place in the building and room number given
//printf( “%s*%s %s %s - %s\n”, subject, courseno, days, from, to );
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

  // ---------- GET ALL SUBJECT/COURSENO/DAYS/TO/FROM FROM THE INTERSECTION INDICIES ---------- //

  //loop through the intersection indices
  //for each index, get the subject/courseno/days/to/from
  //convert it to string for the print function
  //open the instersection file
  FILE *fp = fopen( intersectionFile, "rb" );
  char boolean;
  //for each index in the intersection file:
  for (int i=0; fread(&boolean,1,1,fp)==1; i++)
  {
    //if the line is not empty:
    if (boolean) {
      //for each course number:

      // --- get the subject --- //
      //get index of subject for code i
      getQuery(i, "subject", -1, "subject.set");
      //convert that set (only 1 line) into an int index
      int subjectIndex = set2Index_oneValue("subject.set");
      //get the string value of the subject name at index subjectIndex
      char* subject = getString("subject", subjectIndex);
      // printf("SUBJECT: %s\n", subject);

      // --- get the course number --- //
      //get index of course number for code i
      getQuery(i, "courseno", -1, "courseno.set");
      //convert that set (only 1 line) into an int index
      int courseNumberIndex = set2Index_oneValue("courseno.set");
      //get the string value of the course number name at index courseNumberIndex
      char* courseno = getString("courseno", courseNumberIndex);
      // printf("COURSENO: %s\n", courseno);

      // --- get the days --- //
      //get index of days for code i
      getQuery(i, "days", -1, "days.set");
      //convert that set (only 1 line) into an int index
      int daysIndex = set2Index_oneValue("days.set");
      //get the string value of the days name at index daysIndex
      char* days = getString("days", daysIndex);
      // printf("DAYS: %s\n", days);

      // --- get the from --- //
      //get index of from for code i
      getQuery(i, "from", -1, "from.set");
      //convert that set (only 1 line) into an int index
      int fromIndex = set2Index_oneValue("from.set");
      //get the string value of the from name at index fromIndex
      char* from = getString("from", fromIndex);
      // printf("FROM: %s\n", from);

      // --- get the to --- //
      //get index of to for code i
      getQuery(i, "to", -1, "to.set");
      //convert that set (only 1 line) into an int index
      int toIndex = set2Index_oneValue("to.set");
      //get the string value of the to name at index toIndex
      char* to = getString("to", toIndex);
      // printf("TO: %s\n", to);

      //print based off the assignment requirements 
      printf( "%s*%s %s %s - %s\n", subject, courseno, days, from, to );

      //free each of the malloc'd char* variables
      free(subject);
      free(courseno);
      free(days);
      free(from);
      free(to);
    }
  }

  // ---------- GET ALL SUBJECT/COURSENO/DAYS/TO/FROM FROM THE INTERSECTION INDICIES ---------- //

  // ---------- LAST 20% FUNTONALITY ---------- //


  // ---------- LAST 20% FUNTONALITY ---------- //
  
  return 0;
} 
