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

const string HOME = getenv("HOME"); // user home directory
static bool finished = false;
static const int SCREEN_COLS = engine.getTerminalWidth();
static const int SCREEN_ROWS = engine.getTerminalHeight();

vector<string> album_paths{
    HOME + "/Music/The_Strangers_1979/",
    HOME + "/Music/Les_Failles_2019/",
    HOME + "/Music/Suite_bergamasque_1905/",
};

vector<Album> albums ={};
vector<Song> songs ={}; // songs of current album

string modes = "012";
char currMode = 1; // INDEX of the above string
/*
 *  0 = current song mode (pause / resume / jump)
 *  1 = albums mode (toggle select albums)
 *  2 = songs mode  (toggle select songs within an album)
 *
 */


void mainLoop(){ while(!finished){
	engine.clearScreen();
	engine.drawHorizontalSmoothLine(3, SCREEN_COLS, 5);
	engine.drawVerticalSmoothLine(6, SCREEN_ROWS, SCREEN_COLS-37);
	drawCurrentSong(controller);
	drawAlbums(albums, controller, currMode);
	drawSongsOfAlbum(controller.currAlbum.path, songs, controller, currMode);

	usleep(100000);
    }
}

void addSongProgress(Controller &controller){
    while (!finished){
	if (!controller.isPaused) {
	    controller.currSongProgress++;
	    usleep(1000000);
	} else {
	    usleep(50000);
	}

    }
}

int main(){
    /* change terminal behaviour */
    engine.setCanonicalAndCursor(0);
    engine.clearScreen();
    if (!checkScreenSize()) return 0;

   /* load albums and songs */
    loadAlbums(albums, album_paths);
    controller.currAlbum = albums[0];
    loadSongs(songs, controller.currAlbum.path);

    /* SDL */
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_AudioSpec spec;
    Uint8 * audio_buf;
    Uint32  audio_len;

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
		playSelectedSong(controller, currMode, spec, audio_buf, audio_len);
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

