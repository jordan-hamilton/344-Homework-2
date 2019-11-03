#define MIN_CONNECTIONS 3
#define MAX_CONNECTIONS 6
#define ROOMS_IN_GAME 7
#define TOTAL_ROOMS 10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

struct room {
  char* name;
  char* type;
  int numConnections;
  struct room* connections[MAX_CONNECTIONS];
};

void addRoomConnection(struct room*);
int alreadyConnected(struct room*, struct room*);
void connectRooms(struct room*, struct room*);
int getProcess();
int getRandomInt(int min, int max);
struct room* getRandomRoom(struct room*);
int hasMaxOutboundConnections(struct room*);
void initRooms(struct room*);
void makeDir();
void nameRooms(struct room*);
void makeFiles(struct room*);
int roomConnectionsDone(struct room*);
int sameRoom(struct room*, struct room*);
void setRoomTypes(struct room*);

int main() {
  struct room gameRooms[ROOMS_IN_GAME];
  srand(time(0));

  makeDir();
  initRooms(gameRooms);
  makeFiles(gameRooms);

  return 0;
}

void addRoomConnection(struct room* rooms) {
  /* Credit: Block 2.2 - Program Outlining in Program 2 */

  struct room *roomOne, *roomTwo;

  /* Select a random room, then loop and select a different room if the one we selected already has the maximum
   * number of connections to other rooms. */
  do {
    roomOne = getRandomRoom(rooms);
  } while (hasMaxOutboundConnections(roomOne));

  /* Select a second random room, but loop and select another room if this one has the maximum number of connections
   * to other rooms, is the same as the first room, or already has a connection to the first room we selected. */
  do {
    roomTwo = getRandomRoom(rooms);
  } while (sameRoom(roomOne, roomTwo) || hasMaxOutboundConnections(roomTwo) || alreadyConnected(roomOne, roomTwo));

  /* Connect both rooms once we've ensured they both meet the constraints above. */
  connectRooms(roomOne, roomTwo);
}

int alreadyConnected(struct room* roomOne, struct room* roomTwo) {
  int i;
  /* Loop through all current connections in the first room to verify there isn't already
   * a connection made to a room with the same name as the second room (this works since
   * we ensure the room names randomly chosen each time are unique. */
  for (i = 0; i < roomOne->numConnections; i++) {
    if (strcmp(roomOne->connections[i]->name, roomTwo->name) == 0)
      return 1;
  }
  return 0;
}

void connectRooms(struct room* roomOne, struct room* roomTwo) {
  /* Add a connection to the two rooms passed to the function (in both directions),
   * then increment the number of connections that exist for each room. */
  roomOne->connections[roomOne->numConnections] = roomTwo;
  roomOne->numConnections++;
  roomTwo->connections[roomTwo->numConnections] = roomOne;
  roomTwo->numConnections++;
}

int getProcess() {
  /* Store the current process ID in an integer and return it. */
  int pid = getpid();
  return pid;
}

int getRandomInt(int min, int max) {
  /* Get a random number that's at most the difference between min and max, then add that to the min to get a number
   * in the provided range */
  int randomInt = (rand() % (1 + max - min)) + min;
  return randomInt;
}

struct room* getRandomRoom(struct room* rooms) {
  /* Return a pointer to a random room struct at a valid array index (between 0 and one less than the number of
   * rooms chosen for the game). */
  return &rooms[getRandomInt(0, ROOMS_IN_GAME - 1)];
}

int hasMaxOutboundConnections(struct room* room) {
  /* Verify whether a passed room already has the maximum number of connections to other rooms by checking the
   * numConnections data member. */
  if (room->numConnections == MAX_CONNECTIONS)
    return 1;
  else
    return 0;
}

void initRooms(struct room* rooms) {
  /* Initializes an array of room structs, then names the rooms randomly, sets their types, and randomly connects them.
   * until all rooms have at least the minimum number of connections to other rooms. */
  int i, j;
  for (i = 0; i < ROOMS_IN_GAME; i++) {
    rooms[i].name = NULL;
    rooms[i].type = NULL;
    rooms[i].numConnections = 0;
    for (j = 0; j < MAX_CONNECTIONS; j++) {
      rooms[i].connections[j] = NULL;
    }
  }
  nameRooms(rooms);
  setRoomTypes(rooms);

  while (!roomConnectionsDone(rooms)) {
    addRoomConnection(rooms);
  }
}

void makeDir() {
  /* Allocates memory for a char array, clears out the array, and then prints a formatted string using the
   * required directory prefix followed by the process ID. Finally, a directory is made with that's fully
   * accessible to the user with the formatted string's name, then memory is freed. */
  char* dirName = malloc(36 * sizeof(char));
  if (!dirName)
    printf("An error occurred.\n");

  memset(dirName, '\0', 36);
  sprintf(dirName, "hamiltj2.rooms.%d", getProcess());
  mkdir(dirName, S_IRWXU);

  free(dirName);
}

void makeFiles(struct room* rooms) {
  /* Allocates memory for a formatted file name variable within the directory created by makeDir(),
   * then loops through all rooms for the adventure game and opens a file for each room to add the
   * data associated with that room in the format matching the program specifications. */
  int i, j;
  FILE* roomFile;

  char* fileName = malloc(52 * sizeof(char));
  if (!fileName)
    printf("An error occurred.\n");

  memset(fileName, '\0', 52);

  for (i = 0; i < ROOMS_IN_GAME; i++) {
    sprintf(fileName, "./hamiltj2.rooms.%d/%s_room", getProcess(), rooms[i].name);
    roomFile = fopen(fileName, "a");

    if (!roomFile)
      printf("An error occurred.\n");

    fprintf(roomFile, "ROOM NAME: %s\n", rooms[i].name);

    for (j = 0; j < rooms[i].numConnections; j++) {
      fprintf(roomFile, "CONNECTION %d: %s\n", j + 1, rooms[i].connections[j]->name);
    }

    fprintf(roomFile, "ROOM TYPE: %s\n", rooms[i].type);

    fclose(roomFile);
  }

  free(fileName);
}

void nameRooms(struct room* rooms) {
  /* Hard codes an array of potential room names into an array of strings,
   * then randomly selects one name by its index to assign to a room in the passed array.
   * The string at the selected index is set to NULL to prevent a name from being reused,
   * and the process is repeated in a while loop until all rooms are named. */
  int roomsAdded = 0;
  char* roomNames[TOTAL_ROOMS];

  roomNames[0] = "Kitchen";
  roomNames[1] = "Nursery";
  roomNames[2] = "Bathroom";
  roomNames[3] = "Den";
  roomNames[4] = "Closet";
  roomNames[5] = "Cellar";
  roomNames[6] = "Attic";
  roomNames[7] = "Pantry";
  roomNames[8] = "Bedroom";
  roomNames[9] = "Foyer";

  while (roomsAdded < ROOMS_IN_GAME) {
    int roomToAdd = getRandomInt(0, TOTAL_ROOMS - 1);

    if (roomNames[roomToAdd]) {
      rooms[roomsAdded].name = roomNames[roomToAdd];
      roomNames[roomToAdd] = NULL;
      roomsAdded++;
    }
  }
}

int roomConnectionsDone(struct room* rooms) {
  /* Loops through the passed array to confirm that all rooms have at least the minimum
   * number of connections and not more than the maximum number of connections required
   * to begin creating room files. */
  int i;
  for (i = 0; i < ROOMS_IN_GAME; i++) {
    if (rooms[i].numConnections < MIN_CONNECTIONS || rooms[i].numConnections > MAX_CONNECTIONS)
      return 0;
  }
  return 1;
}

int sameRoom(struct room* roomOne, struct room* roomTwo) {
  /* Returns true or false to indicate whether the passed rooms have the same name,
   * indicating that the room is the same and cannot be connected to itself. */
  if (strcmp(roomOne->name, roomTwo->name) == 0)
    return 1;
  else
    return 0;
}

void setRoomTypes(struct room* rooms) {
  int i;

  /* Loops through all rooms in the array to set the room that was randomly placed at the first index
   * as the start room, the room at the last index as the end room, and all other rooms as mid rooms. */
  for (i = 0; i < ROOMS_IN_GAME; i++) {
    if (i == 0) {
      rooms[i].type = "START_ROOM";
    } else if (i == ROOMS_IN_GAME - 1) {
      rooms[i].type = "END_ROOM";
    } else {
      rooms[i].type = "MID_ROOM";
    }
  }
}
