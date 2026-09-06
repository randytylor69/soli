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

	usleep(10000);
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
		toggleModeChange(controller, currMode);
		break;

	    /* action on currently selected */
	    case '\n':
		if (currMode == 2){
		    /* push current selected to playing */
		   controller.currPlayingAlbum = controller.currAlbum;
		   controller.currPlayingSong = controller.currSong;
		
		    /* load .wav */    
		    SDL_AudioSpec *returnSpec = SDL_LoadWAV(
			controller.currSong.path.c_str(), 
			&spec, &audio_buf, &audio_len
		    );
		    /* opening .wav */
		    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(
			    NULL,0,returnSpec,NULL,0);
		    
		    /* queueing .wav */
		    int queueAudioResult = SDL_QueueAudio(dev, audio_buf, audio_len);
		   SDL_PauseAudioDevice(dev, 0);
		   // SDL_Delay(10000);
		   // SDL_CloseAudioDevice(dev);
		   // SDL_FreeWAV(audio_buf);
		}
		break;
		
	}
    }
    worker.join();
    printf("\n\n\n\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

