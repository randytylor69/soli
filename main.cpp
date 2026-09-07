#include "termigine.h"
#include "soli.h"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <memory>
#include <cstdio>
#include <fstream>
#include <array>
#include <unistd.h>
#include <cmath>
#include <filesystem>
#include <SDL2/SDL.h>
/* clear && make && ./main.out */
using namespace std;

Engine engine;
Controller controller;
sqlite3 *db;

static bool finished = false;
static const int SCREEN_COLS = engine.getTerminalWidth();
static const int SCREEN_ROWS = engine.getTerminalHeight();

// vector<string> album_paths{
//     "/home/ziyin/Music/The_Strangers_1979/",
//     "/home/ziyin/Music/Les_Failles_2019/",
// };
vector<string> album_paths = {};
vector<Album> albums ={};
vector<Song> songs ={}; // songs of current album
char currMode = 1; // 1: album. 2: song. 3: curr song (not used for now)

void mainLoop(){ 
    while(!finished){
	engine.clearScreen();
	engine.drawHorizontalSmoothLine(3, SCREEN_COLS, 5);
	engine.drawVerticalSmoothLine(6, SCREEN_ROWS, SCREEN_COLS-37);
	drawCurrentSong(controller);
	drawAlbums(albums, controller, currMode);
	drawSongsOfAlbum(controller.currAlbum.path, songs, controller, currMode);
	/* check for next song to play */
	checkSongProgress(controller, currMode, songs, albums);

	usleep(250000);
    }
}

void addSongProgress(Controller &controller){
    /* since SDL can't interpret current song progress, like which second it's at, it will be manually calculated */
    while (!finished){
	if (!controller.isPaused) {
	    controller.currSongProgress++;
	    usleep(1000000); // every 60 seconds in Africa, a minute passes
	} else {
	    usleep(50000);
	}

    }
}

int main(int argc, char* argv[]){

    /* change terminal behaviour */
    engine.setCanonicalAndCursor(0);
    engine.clearScreen();
    if (!checkScreenSize()) {
	engine.setCanonicalAndCursor(1);
	return 0;
    };

    
    /* DB operations */
    const char *db_name = "db.sqlite3";
    openDB(db_name, db);
    createTable(db);
    readTable(db, album_paths);
    
    /* TAKING CMD ARGS */
    if (argc > 1) {
	readCMD(argc, argv, engine, db, album_paths);
	return 0;
    };
    /* load albums and songs */
    loadAlbums(albums, album_paths);
    controller.currAlbum = albums[0];
    loadSongs(songs, controller);

    /* SDL */
    SDL_InitSubSystem(SDL_INIT_AUDIO);

    /* multithreading */
    thread worker(mainLoop);
    thread progressWorker(addSongProgress, std::ref(controller));
    while(!finished){
	char ch = getchar();
	switch (ch){

	    case 'q': /* quit program */
		finished = true;
		break;

	    case 'j': /* VM: move down */
		toggleMoveDown(controller, currMode, songs, albums);
		break;

	    case 'k': /* VM: move up */
		toggleMoveUp(controller, currMode, songs, albums);
		break;

	    case '\t': /* toggle mode change */
		toggleModeChange(controller, currMode, songs);
		break;

	    case '\n': /* action on currently selected */

		playSelectedSong(controller, currMode);
		break;

	    case ' ': /* pause currently playing song */
		pauseSelectedSong(controller, currMode);
		break;

	}
    }
    worker.join();
    progressWorker.join();
    printf("\n\n\n\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

