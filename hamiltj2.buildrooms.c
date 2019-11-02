/*
// Created by Jordan Hamilton on 10/27/19.
*/

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
  int id;
  char* name;
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

int main() {
  struct room gameRooms[ROOMS_IN_GAME];
  srand(time(0));

  makeDir();
  initRooms(gameRooms);
  makeFiles(gameRooms);
  
  int i;
  int j;
  for (i = 0; i < ROOMS_IN_GAME; i++) {
    printf("----------------------\n\nRoom: %s\n", gameRooms[i].name);
    for (j = 0; j < gameRooms[i].numConnections; j++) {
      printf("Connection %d: %s\n", j+1, gameRooms[i].connections[j]->name);
    }
  }

  return 0;
}

void addRoomConnection(struct room* rooms) {
  struct room *roomOne, *roomTwo;

  do {
    roomOne = getRandomRoom(rooms);
  } while (hasMaxOutboundConnections(roomOne));

  do {
    roomTwo = getRandomRoom(rooms);
  } while (sameRoom(roomOne, roomTwo) || hasMaxOutboundConnections(roomTwo) || alreadyConnected(roomOne, roomTwo));

  connectRooms(roomOne, roomTwo);
  printf("Room one, %s, is now connected to room two, %s.\n", roomOne->name, roomOne->connections[roomOne->numConnections - 1]->name);
  printf("Room two, %s, is now connected to room one, %s.\n\n", roomTwo->name, roomTwo->connections[roomTwo->numConnections - 1]->name);
}

int alreadyConnected(struct room* roomOne, struct room* roomTwo) {
  int i;
  for (i = 0; i < roomOne->numConnections; i++) {
    if (roomOne->connections[i]->id == roomTwo->id)
      return 1;
  }
  return 0;
}

void connectRooms(struct room* roomOne, struct room* roomTwo) {
  roomOne->connections[roomOne->numConnections] = roomTwo;
  roomOne->numConnections++;
  roomTwo->connections[roomTwo->numConnections] = roomOne;
  roomTwo->numConnections++;
  printf("\nConnected %s to %s\n", roomOne->name, roomTwo->name);
}

int getProcess() {
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
  return &rooms[getRandomInt(0, ROOMS_IN_GAME - 1)];
}

int hasMaxOutboundConnections(struct room* room) {
  if (room->numConnections == 6)
    return 1;
  else
    return 0;
}

void initRooms(struct room* rooms) {
  int i;
  for (i = 0; i < ROOMS_IN_GAME; i++) {
    rooms[i].id = i;
    rooms[i].numConnections = 0;
  }
  nameRooms(rooms);

  while(!roomConnectionsDone(rooms)) {
    printf("Adding a room connection.\n");
    addRoomConnection(rooms);
  }
}

void makeDir() {
  char* dirName = malloc(21 * sizeof(char));
  if (dirName == NULL)
    printf("An error occurred.\n");

  memset(dirName, '\0', 21);
  sprintf(dirName, "hamiltj2.rooms.%d", getProcess());
  mkdir(dirName, S_IRWXU);

  free(dirName);
}

void makeFiles(struct room* gameRooms) {
  int i;
  for (i = 0; i < ROOMS_IN_GAME; i++)
    printf("Index: %d | Room name: %s\n", gameRooms[i].id, gameRooms[i].name);
}

void nameRooms(struct room* rooms) {
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

    if (roomNames[roomToAdd] != NULL) {
      rooms[roomsAdded].name = roomNames[roomToAdd];
      roomNames[roomToAdd] = NULL;
      printf("Added %s to index %d\n", rooms[roomsAdded].name, roomsAdded);
      roomsAdded++;
    }
  }
}

int roomConnectionsDone(struct room* rooms) {
  int i;
  for (i = 0; i < ROOMS_IN_GAME; i++) {
    if (rooms[i].numConnections < MIN_CONNECTIONS || rooms[i].numConnections > MAX_CONNECTIONS)
      return 0;
  }
  return 1;
}

int sameRoom(struct room* roomOne, struct room* roomTwo) {
  if (roomOne->id == roomTwo->id)
    return 1;
  else
    return 0;
}
