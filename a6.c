#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

// TO TEST, RUN:
//make
//./a6 ALEX 200 > test/output.txt
//diff -e test/output.txt test/expectedOutput_80%.txt && echo $'\nCOREECT OUTPUT\n'
//or in one command:
//make && ./a6 ALEX 200 > test/output.txt && diff -e test/output.txt test/expectedOutput_80%.txt && echo $'\nCOREECT OUTPUT\n'

//TO TEST WITH VALGRIND
//valgrind --log-file=logFile1.txt --track-origins=yes --leak-check=full -s --leak-check=full --show-leak-kinds=all ./a6 ALEX 200

// ---------- FUNCTIONS USED FROM PROFESSOR ---------- //

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

  //free what is malloced so there are no memory leaks
  free(array);

  fclose( fp );
}

//step 5
//create an output set file that is the intersection of setFile1 and setFile2
int and(char* setFile1, char* setFile2, char* outputFile) {
  FILE *fp1 = fopen( setFile1, "rb" );
  FILE *fp2 = fopen( setFile2, "rb" );
  FILE *fp3 = fopen( outputFile, "wb" );
  char b1, b2, b3;

  int sizeOfIntersection = 0;

  while ( fread( &b1, 1, 1, fp1 )==1 && fread( &b2, 1, 1, fp2 ) )
  {
    b3 = b1&&b2;
    fwrite( &b3, 1, 1, fp3 );
    if (b3 == 1) {
      sizeOfIntersection++;
    }

  }

  fclose( fp1 );
  fclose( fp2 );
  fclose( fp3 );

  return sizeOfIntersection;
}

//return the index from a file 
//only for one line
//used to read the subject/courseno/days/to/from values of each line in the intersection
int set2Index_oneValue(char* filename) {
  char boolean;

  FILE *fp = fopen( filename, "rb" );

  for (int i=0; fread(&boolean,1,1,fp)==1; i++)
  {
    if (boolean) {
      //close file so there is no memory leak
      fclose(fp);
      return i;
    }
  }
  //close file so there is no memory leak
  fclose(fp);
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

  buffer[strlen(buffer)-1] = '\0';

  return buffer;
}

//initialize Set struct
struct Set {
  int capacity; //total number of elements
  char** table; //hash table
};

//create an empty Set
struct Set *empty(int capacity){
    struct Set *ptr;

    ptr = malloc(sizeof(struct Set));
    if(!ptr){
        fprintf(stderr, "Malloc failed\n");
        exit(-1);
    }

    ptr -> capacity = capacity;
    ptr -> table = malloc(sizeof(char *) * capacity);

    if(!ptr->table){
        fprintf(stderr, "Malloc failed\n");
        exit(-1);
    }

    for(int i = 0; i < capacity; i++){
        ptr -> table[i] = NULL;
    }

    return ptr;
}

//check if the element is already in the set
int is_member(struct Set *ptr, char* element) {
  int hash_idx = str2int(element, ptr->capacity);
  
  int i;
  i = hash_idx;

  do {
    if (ptr->table[i]) {
      if (strcmp(element, ptr->table[i]) == 0) {
        return 1;
      }
    }
    i++;
    if (i == ptr->capacity) {
      i = 0;
    }
  } while (i != hash_idx);


  return 0;
}

//add the element in the set
//only if it is not already in the set (no duplicates)
void add_element(struct Set *ptr, char *element){
    if (is_member(ptr, element)){
        return;
    }

    int hash_idx = str2int(element, ptr -> capacity);

    while (ptr -> table[hash_idx]){
        hash_idx++;
        if (hash_idx == ptr -> capacity){
            hash_idx = 0;
        }
    }

    ptr -> table[hash_idx] = element; 
}

//print the set
void print_set(struct Set *ptr){
    printf("{ ");
    for (int i = 0; i < ptr -> capacity; i++){
        if (ptr -> table[i]){
            printf("%s, ", ptr -> table[i]);
        }
    }
    printf("\b\b }");
}

//free the set
void free_set(struct Set *ptr){
    free(ptr -> table);
    free(ptr);
}

// ---------- FUNCTIONS USED FROM PROFESSOR ---------- //

//returns the String value of the element at given index
char* getElementForPrint(int index, char* elementName, char* setName) {

  //get index of the element for code i
  getQuery(index, elementName, -1, setName);
  //convert that set (only 1 line) into an int index
  int elementIndex = set2Index_oneValue(setName);
  //get the string value of the element name at index elementIndex
  char* element = getString(elementName, elementIndex);
  // printf("%s: %s\n", elementName, element);

  return element;
}

//copy the classes subject/courseno/from/to into the corresponding day array 
void getInfoIntoClass(char classes[][4][8], int index, char* subject, char* courseno, char* from, char* to){
    strcpy(classes[index][0], subject);
    strcpy(classes[index][1], courseno);
    strcpy(classes[index][2], from);
    strcpy(classes[index][3], to);
}

//orders the array of classes based off of start time (bubble sort)
void orderedClasses(char classes[][4][8], int counter) {
    //temp variables used for swapping
    char swapSubject[5];
    char swapCourseno[5];
    char swapFrom[8];
    char swapTo[8];

    //boolean; 1 = a swap is required
    int swap = 0;

    //for all classes in the array (squared)
    for (int i = 0; i < counter - 1; i++) {
        for (int j = 0; j < counter - i - 1; j++) {
            //if the first class is PM and the second class is AM, they automatically switch
            if (classes[j][2][5] == 'P' && classes[j+1][2][5] == 'A'){
                swap = 1;
            }
            //otherwise: same 12hr timezone(AM/PM)
            else if (classes[j][2][5] == classes[j+1][2][5]){
                //special case for 12
                if ((classes[j][2][0] == '1' && classes[j][2][1] == '2') && (classes[j+1][2][0] != '1' || classes[j+1][2][1] != '2')){
                    swap = 0;
                }
                else if ((classes[j][2][0] != '1' || classes[j][2][1] != '2') && (classes[j+1][2][0] == '1' && classes[j+1][2][1] == '2')){
                    swap = 1;
                }
                //all other cases:
                else{
                    //for each digit of the "from" time
                    for (int k = 0; k < 5; k++) {
                        if (classes[j][2][k] > classes[j+1][2][k]){
                            swap = 1;
                            break;
                        }
                        else if (classes[j][2][k] < classes[j+1][2][k]){
                            break;
                        }
                    }
                }
            }

            //if a swap is required
            if (swap == 1) {
                //class class1 in temp
                strcpy(swapSubject, classes[j][0]);
                strcpy(swapCourseno, classes[j][1]);
                strcpy(swapFrom, classes[j][2]);
                strcpy(swapTo, classes[j][3]);

                //copy class2 to class1
                strcpy(classes[j][0], classes[j+1][0]);
                strcpy(classes[j][1], classes[j+1][1]);
                strcpy(classes[j][2], classes[j+1][2]);
                strcpy(classes[j][3], classes[j+1][3]);

                //copy temp (class1) to class2
                strcpy(classes[j+1][0], swapSubject);
                strcpy(classes[j+1][1], swapCourseno);
                strcpy(classes[j+1][2], swapFrom);
                strcpy(classes[j+1][3], swapTo);
            }

            //reinitialize the boolean each loop
            swap = 0;
        }
    }
}

//prints the array of classes based off assignment description
void printClass(char* dayOfTheWeek, char class[][4][8], int counter) {
    for (int i = 0; i < counter; i++){
        printf("%s*%s %s %s - %s\n", class[i][0], class[i][1], dayOfTheWeek, class[i][2], class[i][3]);
    }
}

//given command line arguments <buildingName> <roomNumber>
//outputs all the courses that take place in the building and room number given
//printf( ???%s*%s %s %s - %s\n???, subject, courseno, days, from, to );
//for additional functionality; gets rid of all duplicate entries 
//and prints the info based of the days of the week and start time
int main( int argc, char **argv ) {
  
  //value of indicies after hash_lookup()
  long indexBuilding;
  long indexRoom;

  //the building and room value given by argv[1] & argv[2]
  char* buildingValue = argv[1];
  char* roomValue = argv[2];

  //file name constants
  char* buildingFile = "building.set";
  char* roomFile = "file.set";
  char* intersectionFile = "intersection.set";

  //number of elements in the intersection file
  int sizeOfIntersection = 0;  

  //set used to get rid of duplicate entries
  struct Set* noDuplicates;

  //make sure user inputs the correct command line (+ arguments)
  if (argc!=3) {
    fprintf( stderr, "Usage: %s <buildingName> <roomNumber>\n", argv[0] );
    exit(-1);
  }

  // ---------- GET INDEX OF BUILDING ---------- //

  indexBuilding = getIndex("building", buildingValue);
  // printf( "Index of building %s: %ld\n",buildingValue, indexBuilding );

  // ---------- GET INDEX OF ROOM ---------- //

  indexRoom = getIndex("room", roomValue);
  // printf( "Index of room %s: %ld\n", roomValue, indexRoom );

  // ---------- SET FILE OF INDICES OF ALL COURSES IN BUILDING_INDEX ---------- //

  getQuery(-1, "building", indexBuilding, buildingFile); 

  // ---------- SET FILE OF INDICES OF ALL COURSES IN ROOM_INDEX ---------- //

  getQuery(-1, "room", indexRoom, roomFile); 

  // ---------- SET FILE OF INDICES OF INTERSECTION OF BUILDING_FILE AND ROOM_FILE ---------- //
  
  sizeOfIntersection = and(buildingFile, roomFile, intersectionFile);

  // ---------- VARIABLES USED FOR THE LAST 20% FUNTONALITY ---------- //

  //set that has all classes without duplicates
  //make it size of intersection because it needs to be statically allocated memory 
  //at most will have all elements in the intersection (there is actually no duplicates)
  noDuplicates = empty(sizeOfIntersection);

  //courseno used to add to the noDuplicates
  char elementForSet[sizeOfIntersection][7];
  //number of elements in the noDuplicate set
  int noDuplicatesCounter = 0;

  //string array for each class info part (subject, courseno, to, from) for each day of the week
  char mondayClasses[sizeOfIntersection][4][8];
  char tuesdayClasses[sizeOfIntersection][4][8];
  char wednesdayClasses[sizeOfIntersection][4][8];
  char thursdayClasses[sizeOfIntersection][4][8];
  char fridayClasses[sizeOfIntersection][4][8];
  char saturdayClasses[sizeOfIntersection][4][8];


  //initialize all the arrays to zero so there are no memory leaks
  memset(mondayClasses, 0, sizeof(mondayClasses));
  memset(tuesdayClasses, 0, sizeof(tuesdayClasses));
  memset(wednesdayClasses, 0, sizeof(wednesdayClasses));
  memset(thursdayClasses, 0, sizeof(thursdayClasses));
  memset(fridayClasses, 0, sizeof(fridayClasses));
  memset(saturdayClasses, 0, sizeof(saturdayClasses));
   

  //counter for the number of classes in each day of the week
  int mondayClassesCounter = 0;
  int tuesdayClassesCounter = 0;
  int wednesdayClassesCounter = 0;
  int thursdayClassesCounter = 0;
  int fridayClassesCounter = 0;
  int saturdayClassesCounter = 0;  
  
  //loop through the intersection indices
  //for each index, get the subject/courseno/days/to/from
  //convert it to string for the print function
  //open the instersection file
  FILE *fp = fopen( intersectionFile, "rb" );
  char boolean;
  //for each index in the intersection file:
  for (int i=0; fread(&boolean,1,1,fp)==1; i++) {
    //if the line is not empty:
    if (boolean) {

      // ---------- GET ALL SUBJECT/COURSENO/DAYS/TO/FROM FROM THE INTERSECTION INDICIES ---------- //

      //for each course number:

      // --- get the subject --- //
      char* subject = getElementForPrint(i, "subject", "subject.set");

      // --- get the course number --- //
      char* courseno = getElementForPrint(i, "courseno", "courseno.set");

      // --- get the days --- //
      char* days = getElementForPrint(i, "days", "days.set");

      // --- get the from time --- //
      char* from = getElementForPrint(i, "from", "form.set");

      // --- get the to time --- //
      char* to = getElementForPrint(i, "to", "to.set");

      // ---------- LAST 20% FUNTONALITY ---------- //

      // ---------- GET RID OF DUPLICATES ---------- //
      
      //if the class is already in the set:
      //do not add it to the set:
      //so you can free all the local variables
      if (is_member(noDuplicates, courseno)) {
        //free each of the malloc'd char* variables
        free(subject);
        free(courseno);
        free(days);
        free(from);
        free(to);
      } 
      //otherwise:
      //it is a unique class
      else {
        //add the class to the set
        strcpy(elementForSet[noDuplicatesCounter], courseno);
        add_element(noDuplicates, elementForSet[noDuplicatesCounter]);
        noDuplicatesCounter++;

        // ---------- PARTITION CLASSES BASED OFF DAYS OF THE WEEK ---------- //

        //use pointer arithmetic to check what day of the week it belongs to
        char* firstLetterPointer = &days[0];
        //boolean: 0 = there are still more days of the week this class is in
        int lastDay = 0; 

        //input days are ordered 
        
        //if in Monday:
        if (*firstLetterPointer == 'M') {
          //add to the Monday class array 
          getInfoIntoClass(mondayClasses, mondayClassesCounter, subject, courseno, from, to);
          mondayClassesCounter++;

          //if there is a another day the class takes place:
          //3 because "Mon,"
          if (*(firstLetterPointer + 3) == ',') {
            //5 because "Mon, x"
            firstLetterPointer += 5; 
          }
          //otherwise: no other days for the class
          else {
            lastDay = 1;
          }
        }

        //if in Tuesday:
        if (*firstLetterPointer == 'T' && *(firstLetterPointer + 1) == 'u' && lastDay == 0) {
          //add to the Tuesday class array
          getInfoIntoClass(tuesdayClasses, tuesdayClassesCounter, subject, courseno, from, to);
          tuesdayClassesCounter++;

          //if there is a another day the class takes place:
          //4 because "Tues,"
          if (*(firstLetterPointer + 4) == ',') {
            //6 because "Tues, x"
            firstLetterPointer += 6; 
          }
          //otherwise: no other days for the class
          else {
            lastDay = 1;
          }
        }
        //if in Wednesday:
        if (*firstLetterPointer == 'W' && lastDay == 0) {
          //add to the Wednesday class array
          getInfoIntoClass(wednesdayClasses, wednesdayClassesCounter, subject, courseno, from, to);
          wednesdayClassesCounter++;

          //if there is a another day the class takes place:
          //3 because "Wed,"
          if (*(firstLetterPointer + 3) == ',') {
            //5 because "Wed, x"
            firstLetterPointer += 5; 
          }
          //otherwise: no other days for the class
          else {
            lastDay = 1;
          }
        }
        //if in Thursday
        if (*firstLetterPointer == 'T' && *(firstLetterPointer + 1) == 'h' && lastDay == 0) {
          //add to the Thursday class array
          getInfoIntoClass(thursdayClasses, thursdayClassesCounter, subject, courseno, from, to);
          thursdayClassesCounter++;

          //if there is a another day the class takes place:
          //4 because "Thurs,"
          if (*(firstLetterPointer + 4) == ',') {
            //5 because "Thurs, x"
            firstLetterPointer += 6; 
          }
          //otherwise: no other days for the class
          else {
            lastDay = 1;
          }
        }
        //if in Friday
        if (*firstLetterPointer == 'F' && lastDay == 0) {
          //add to the Friday class array
          getInfoIntoClass(fridayClasses, fridayClassesCounter, subject, courseno, from, to);
          fridayClassesCounter++;

          //if there is a another day the class takes place:
          //3 because "Fri,"
          if (*(firstLetterPointer + 3) == ',') {
            //5 because "Fri, x"
            firstLetterPointer += 5; 
          }
          else {
            //otherwise: no other days for the class
            lastDay = 1;
          }
        }
        //if in Saturday
        if (*firstLetterPointer == 'S' && lastDay == 0) {
          //add to the Saturday class array
          getInfoIntoClass(saturdayClasses, saturdayClassesCounter, subject, courseno, from, to);
          saturdayClassesCounter++;
        }
        //Saturday is the last possible day therefore we finish

        //finished adding the class to set and classes
        //can now free the local variable
        free(subject);
        free(courseno);
        free(days);
        free(from);
        free(to);
      } 
    }
  }

  //free the allocated spacec for the noDuplicates set
  free_set(noDuplicates);
  //close the file so there is no more memory leaks
  fclose(fp);

  // ---------- ORGANIZE BY TIME ---------- //

  // ---------- VARIABLES USED FOR ORGANIZING BY TIME ---------- //

  //2D array of number of classes in each day of the week
  int allCounters[6] = {mondayClassesCounter, tuesdayClassesCounter, wednesdayClassesCounter, thursdayClassesCounter, fridayClassesCounter, saturdayClassesCounter};

  //4D array of all classes for each day of the week
  char allClasses[6][sizeOfIntersection][4][8];

  //need to use strcpy for "strings" in c
  //for each dimension of the array
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j<allCounters[i]; j++) {
      for (int k = 0; k<4; k++) {
        for (int l = 0; l<8; l++) {
          strcpy(&allClasses[0][j][k][l], &mondayClasses[j][k][l]);
          strcpy(&allClasses[1][j][k][l], &tuesdayClasses[j][k][l]);
          strcpy(&allClasses[2][j][k][l], &wednesdayClasses[j][k][l]);
          strcpy(&allClasses[3][j][k][l], &thursdayClasses[j][k][l]);
          strcpy(&allClasses[4][j][k][l], &fridayClasses[j][k][l]);
          strcpy(&allClasses[5][j][k][l], &saturdayClasses[j][k][l]);

        }
      }
    }
  }

  char* allDays[6] = {"Mon", "Tues", "Wed", "Thur", "Fri", "Sat"};

  //for all classes array for each day of the week
  for (int dayOfTheWeek = 0; dayOfTheWeek < 6; dayOfTheWeek++) {
    //order the classes in the day of the week array based off of start time
    orderedClasses(allClasses[dayOfTheWeek], allCounters[dayOfTheWeek]);
    //print all of the classes based of the days of the week and start times
    printClass(allDays[dayOfTheWeek], allClasses[dayOfTheWeek], allCounters[dayOfTheWeek]);
  }

  return 0;
} 

