#define MAX_CONNECTIONS 6
#define ROOMS_IN_GAME 7

#include <dirent.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Create the room struct with an array of room names this time, since we would have to read each file multiple times
 * to populate the connections with pointers to rooms. */
struct room {
  char* name;
  char* type;
  int numConnections;
  char* connections[MAX_CONNECTIONS];
};

pthread_t gameClock;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

char* getLatestDir();
void cleanUp(struct room*);
void initRooms(struct room*);
void parseFile(FILE*, struct room*, int roomNum);
void play(struct room*);
void readFiles(char*, struct room*);
void* setTime();

int main() {
  int threadResult;
  struct room* gameRooms;
  gameRooms = malloc(ROOMS_IN_GAME * sizeof(struct room));

  /* Lock the mutex we created and create a thread. */
  pthread_mutex_lock(&mutex);
  threadResult = pthread_create(&gameClock, NULL, setTime, &mutex);
  if (threadResult != 0)
    printf("An error occurred.\n");

  initRooms(gameRooms);
  readFiles(getLatestDir(), gameRooms);

  play(gameRooms);

  cleanUp(gameRooms);

  /* Unlock and destroy the mutex. */
  pthread_mutex_unlock(&mutex);
  pthread_mutex_destroy(&mutex);
  return 0;
}

void cleanUp(struct room* rooms) {
  /* Free memory that was allocated for the name of each room, its type, and the string containing the name of each
   * connected room. Finally, free the memory allocated for the room struct itself. */
  int i, j;
  for (i = 0; i < ROOMS_IN_GAME; i++){
    free(rooms[i].name);
    free(rooms[i].type);
    for (j = 0; j < rooms[i].numConnections; j++)
      free(rooms[i].connections[j]);
  }
  free(rooms);
}

char* getLatestDir() {
  /* Credit: Block 2.4 - Manipulating Directories
   * Tracks the latest timestamp of the subdirectories that match the supplied prefix, then stores the subdirectory's
   * name in a string and returns it. */
  int timestamp = -1;
  DIR* currentDir;
  struct dirent* subDir;
  struct stat attributes;

  char* prefix = "hamiltj2.rooms.";
  char* latestDir = malloc(36 * sizeof(char));
  memset(latestDir, '\0', 36);

  currentDir  = opendir(".");
  if (!currentDir)
    printf("An error occurred.\n");

  while ((subDir = readdir(currentDir))) {
    if (strstr(subDir->d_name, prefix)) {
      stat(subDir->d_name, &attributes);

      if ((int) attributes.st_mtime > timestamp) {
        timestamp = (int) attributes.st_mtime;
        memset(latestDir, '\0', 36);
        strcpy(latestDir, subDir->d_name);
      }

    }
  }

  closedir(currentDir);
  return latestDir;
}

void initRooms(struct room* rooms) {
  /* Initializes an array of room structs by allocating memory for a name and type, then setting pointers to each room
   * connection to NULL, since we'll allocate memory for the exact number of connections when reading room files.
   * The name, type and connections are established in parseFile. Memory is freed in the cleanUp function. */
  int i, j;

  for (i = 0; i < ROOMS_IN_GAME; i++) {
    rooms[i].name = malloc(9 * sizeof(char));
    memset(rooms[i].name, '\0', 9);
    rooms[i].type = malloc(11 * sizeof(char));
    memset(rooms[i].type, '\0', 11);
    rooms[i].numConnections = 0;
    for (j = 0; j < MAX_CONNECTIONS; j++) {
      rooms[i].connections[j] = NULL;
    }
  }
}

void parseFile(FILE* roomFile, struct room* rooms, int roomNum) {
  /* Allocates memory to store the first and last word on each line in a file, then stores the room name in the first
   * unused struct in the rooms array (at the index passed as roomNum). After storing the name, fscanf reads subsequent
   * lines and stores the first and words on each line in the corresponding variables, ignoring the second word. If the
   * first word matches CONNECTION, we allocate memory for a string and add a connection to the list of connections for
   * the room we're reading, then increment the number of connections. Otherwise, we assume that this is the last line
   * in the file indicating the room type, and store that as the room struct's type, then exit the loop and free memory.*/
  int doneReading = 0;
  char* connection = "CONNECTION";

  char* firstWord = malloc(11 * sizeof(char));
  char* lastWord = malloc(11 * sizeof(char));
  memset(firstWord, '\0', 11);
  memset(lastWord, '\0', 11);

  fscanf(roomFile, "%*s %*s %s", lastWord);
  strcpy(rooms[roomNum].name, lastWord);

  do {
    memset(firstWord, '\0', 11);
    memset(lastWord, '\0', 11);
    fscanf(roomFile, "%s %*s %s", firstWord, lastWord);

    if (strcmp(connection, firstWord) == 0) {
      rooms[roomNum].connections[rooms[roomNum].numConnections] = malloc(9 * sizeof(char));
      memset(rooms[roomNum].connections[rooms[roomNum].numConnections], '\0', 9);
      strcpy(rooms[roomNum].connections[rooms[roomNum].numConnections], lastWord);
      rooms[roomNum].numConnections++;
    } else {
      strcpy(rooms[roomNum].type, lastWord);
      doneReading = 1;
    }

  } while(!doneReading);

  free(firstWord);
  free(lastWord);
}

void play(struct room* rooms) {
  FILE* timeFile;
  int i = 0, j = 0;
  int charsEntered = -5, stepsTaken = 0, validInput = 0;
  int threadResult = 0;
  char* getTime = "time";
  char* timeFilePath = "./currentTime.txt";
  char timeOutput[256];
  char* startingPos = "START_ROOM";
  char* endingPos = "END_ROOM";
  char* input = NULL;
  size_t inputSize = 0;
  struct room* history[100];
  struct room* position = &rooms[0];

  /* Set the starting room to the room struct whose type is equal to START_ROOM. */
  while (strcmp(position->type, startingPos) != 0) {
    position = &rooms[++i];
  }

  do {
    /* Reset whether the user's input valid by assuming it is false. */
    validInput = 0;

    /* Display the current location to the user, then loop through that position's list of connections to display valid
     * rooms where the user can move. */
    printf("CURRENT LOCATION: %s\nPOSSIBLE CONNECTIONS: ", position->name);
    for (i = 0; i < position->numConnections; i++) {
      printf("%s", position->connections[i]);
      if (i == position->numConnections - 1)
        printf(".\n");
      else
        printf(", ");
    }

    /* Prompt for the next location, then store the user's input in the input buffer. Ensure there's a null terminator
     * using the trick from the sample userinput.c program. */
    printf("WHERE TO? >");
    charsEntered = getline(&input, &inputSize, stdin);
    input[charsEntered - 1] = '\0';


    while(strcmp(input, getTime) == 0) {
      /* Check if the user requests the time before comparing against other input so we can unlock the mutex, wait for
       * the second thread to terminate, lock the mutex again and create another thread so the time can be set again. */
      pthread_mutex_unlock(&mutex);
      pthread_join(gameClock, NULL);
      pthread_mutex_lock(&mutex);
      threadResult = pthread_create(&gameClock, NULL, setTime, &mutex);

      /* Print an error message if we were not able to create a new thread successfully. */
      if (threadResult != 0)
        printf("An error occurred.\n");

      memset(timeOutput, '\0', sizeof(timeOutput));

      timeFile = fopen(timeFilePath, "r");
      fgets(timeOutput, sizeof(timeOutput), timeFile);
      fclose(timeFile);

      printf("\n%s\n", timeOutput);

      printf("WHERE TO? >");
      charsEntered = getline(&input, &inputSize, stdin);
      input[charsEntered - 1] = '\0';
    }

    for (i = 0; i < position->numConnections; i++) {
      if (strcmp(position->connections[i], input) == 0) {
        /* Consider the user's input to be valid since we've confirmed that it was a room that was in the provided
         * list of connections. */
        validInput = 1;

        for(j = 0; j < ROOMS_IN_GAME; j++) {
          /* Loop through all structs in the game's rooms, checking for the room name that matches the user's input.
           * Once found, update the user's current position, add its pointer to the last unused index in the history
           * array, and increment the number of steps taken since we've successfully moved to a new room. */
          if (strcmp(rooms[j].name, input) == 0) {
            position = &rooms[j];
            history[stepsTaken] = &rooms[j];
            stepsTaken++;
          }
        }

      }
    }

    /* If the validInput was never updated to 1 once we checked the list of connections from the current position
     * against the user's input, display an error message and return to the top of the loop. */
    if (!validInput)
      printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");

    /* Clean up formatting by ensuring we always have a new line before printing the current location and list
     * of connections again, or before printing the congratulations message. */
    printf("\n");

  /* Repeat the prompt for a new location as long as the user's current position was not set to the END_ROOM. */
  } while (strcmp(position->type, endingPos) != 0);

  /* Print the congratulations message, then loop through the history array and print the name in the struct pointer we
   * stored after each step was taken. */
  printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\nYOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n", stepsTaken);
  for (i = 0; i < stepsTaken; i++)
    printf("%s\n", history[i]->name);

  /* Free memory allocated for the user input buffer. */
  free(input);
}

void readFiles(char* dirPath, struct room* rooms) {
  /* Open the directory at the path passed from the getLatestDir function, then use the same logic from Block 2.4 -
   * Manipulating Directories to loop through the files in this directory. As long as a file ends in _room, open the file
   * and pass the stream to parseFile to form a struct from its contents and finally close the file and increment the number
   * of room files we've read in this directory. Clean up by closing the directory and free memory for strings that
   * held the path to the latest directory and the full path to each room file. */
  int roomsRead = 0;
  DIR* currentDir;
  FILE* currentFile;
  struct dirent* roomFile;
  char* filePath = malloc(52 * sizeof(char));
  memset(filePath, '\0', 52);

  currentDir = opendir(dirPath);
  if (!currentDir)
    printf("An error occurred.\n");

  while ((roomFile = readdir(currentDir))) {
    if (strstr(roomFile->d_name, "_room")) {
      sprintf(filePath, "%s/%s", dirPath, roomFile->d_name);
      currentFile = fopen(filePath, "r");

      if (!currentFile)
        printf("An error occurred.\n");

      parseFile(currentFile, rooms, roomsRead);
      fclose(currentFile);
      roomsRead++;
    }
  }

  closedir(currentDir);
  free(filePath);
  free(dirPath);
}

void* setTime() {
  pthread_mutex_lock(&mutex);
  FILE* timeFile;
  struct tm* calendarTime;
  char* fileName = "./currentTime.txt";
  char formattedTime[256];
  memset(formattedTime, '\0', 256);

  /* Get the current time*/
  time_t currentTime = time(0);
  /* Get a struct with the calendar date based off of the current time to begin formatting with strftime */
  calendarTime = localtime(&currentTime);

  /* Format the string based off of the struct using these character sequences:
   * https://linux.die.net/man/3/strftime */
  strftime(formattedTime, sizeof(formattedTime), "%l:%M%P, %A, %B %d, %Y", calendarTime);

  timeFile = fopen(fileName, "w");
  fprintf(timeFile, "%s\n", formattedTime);
  fclose(timeFile);

  pthread_mutex_unlock(&mutex);
  /* Return a null pointer since we needed a function that returned a void pointer. */
  return NULL;
}
