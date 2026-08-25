#define MINIAUDIO_IMPLEMENTATION
#include "termigine.h"
#include "soli.h"
#include "miniaudio.h"
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

using namespace std;

ma_engine miniaudio;
ma_sound currentSound;

Engine engine;
Controller controller;

const string HOME = getenv("HOME");
static bool finished = false;
static const int SCREEN_COLS = engine.getTerminalWidth();
static const int SCREEN_ROWS = engine.getTerminalHeight();

vector<string> album_paths{
    HOME + "/Music/Amore_1982/",
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


void mainLoop(){
    while(!finished){
	engine.clearScreen();
	engine.drawHorizontalSmoothLine(3, SCREEN_COLS, 5);
	engine.drawVerticalSmoothLine(6, SCREEN_ROWS, SCREEN_COLS-37);
	drawCurrentSong(controller);
	drawAlbums(albums, controller, currMode);
	drawSongsOfAlbum(controller.currAlbum.path, songs, controller, currMode);

	usleep(10000);
    }
}

int main(){
    /* change terminal behaviour */
    engine.setCanonicalAndCursor(0);
    engine.clearScreen();
    if (!checkScreenSize()) return 0;
    ma_engine_init(NULL, &miniaudio);

    /* load albums and songs */
    loadAlbums(albums, album_paths);
    controller.currAlbum = albums[0];
    loadSongs(songs, controller.currAlbum.path);

    /* multithreading */
    thread worker(mainLoop);
    while(!finished){
	char ch = getchar();
	switch (ch){

	    /* quit the program */
	    case 'q':
		finished = true;
		break;

	    /* VM: move down */
	    case 'j':
		if (currMode==1){
		    controller.currAlbumIndex=(controller.currAlbumIndex+1)%albums.size();
		    controller.currAlbum = albums[controller.currAlbumIndex];
		    loadSongs(songs, controller.currAlbum.path);
		} else if (currMode==2){
		    controller.currSongIndex=(controller.currSongIndex+1)%songs.size();
		    controller.currSong = songs[controller.currSongIndex];
		}
		break;

	    /* VM: move up */
	    case 'k':
		if (currMode==1){
		    controller.currAlbumIndex=
			controller.currAlbumIndex==0?
			    albums.size()-1 : controller.currAlbumIndex-1;
		    controller.currAlbum = albums[controller.currAlbumIndex];
		    loadSongs(songs, controller.currAlbum.path);
		} else if (currMode==2){
		    controller.currSongIndex=
			controller.currSongIndex==0?
			    songs.size()-1: controller.currSongIndex-1;
		    controller.currSong = songs[controller.currSongIndex];
		} 
		break;

	    /* change mode */
	    case '\t':
		currMode = (currMode+1)%3;
		if (currMode == 2){
		    controller.currSongIndex = 0; // reset to the first song
		}
		break;

	    /* action on currently selected */
	    case '\n':
		if (currMode == 2){
		   controller.currPlayingAlbum = controller.currAlbum;
		   controller.currPlayingSong = controller.currSong;

		   if (controller.isPlaying){
		       /* if something's playing, remove it */
		       ma_sound_uninit(&currentSound); 
		       controller.isPlaying = false;
		   }

		   if (ma_sound_init_from_file(&miniaudio, (controller.currPlayingSong->path).c_str(), MA_SOUND_FLAG_STREAM, NULL, NULL, &currentSound)==MA_SUCCESS){
		       ma_sound_start(&currentSound);
		       controller.isPlaying = true;
		   }
		}
		break;
		
	}
    }
    worker.join();
    printf("\n\n\n\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

