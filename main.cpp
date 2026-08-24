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
using namespace std;

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

void loadAlbums(vector<Album> &albums, vector<string> album_paths){
    for (const string &path : album_paths){

	/* 1. Get album art escape sequence to cache */
	string cmd ="chafa --format=kitty -s 7x7 "+path+"cover.jpg"; 
	array<char, 128> buffer;
	string result;
	auto pipeCloser = [](FILE* fp) { (void)pclose(fp); };
	unique_ptr<FILE, decltype(pipeCloser)> pipe(popen(cmd.c_str(), "r"), pipeCloser);
	while(fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get())!=nullptr){
	    result+=buffer.data();
	}

	/* 2. Get album name + artist + year */
	vector<string> albuminfo = {"Unknown", "Unknown", "Unknown"};
	int infoLineCount = 0;
	ifstream f(path+"metadata.txt");
	string l;
	while(getline(f, l)){
	    albuminfo[infoLineCount]=l;
	    infoLineCount++;
	}

	/* 3. Add new album instance */
	albums.push_back(
	    Album(path, albuminfo[0], albuminfo[1], albuminfo[2], result)
	);
    }
}

void loadSongs(vector<Song> &songs, string album_path){
    songs = {}; 

    for (auto &file : filesystem::directory_iterator(album_path)){
	string fname = file.path().filename().string();
	if (fname.substr(fname.length()-4, 4)!=".mp3") continue;
	
	fname = fname.substr(0, fname.length()-4); // rm extension
	if (fname.length() > 45) { // abbreviate if name too long
	    fname = fname.substr(0, 44) + "...";
	}

	songs.push_back(Song(file.path(), fname));
    }
}

void mainLoop(){
    while(!finished){
	engine.clearScreen();
	engine.drawHorizontalSmoothLine(3, SCREEN_COLS, 5);
	engine.drawVerticalSmoothLine(6, SCREEN_ROWS, SCREEN_COLS-37);
	drawCurrentSong("[SONG]", "[ALBUM]", "[YEAR]", "[ARTIST]");
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

    /* multithreading */
    thread worker(mainLoop);
    while(!finished){
	char ch = getchar();
	switch (ch){
	    case 'q':
		finished = true;
		break;
	    case 'j':
		if (currMode==1){
		    controller.currAlbumIndex=(controller.currAlbumIndex+1)%albums.size();
		    controller.currAlbum = albums[controller.currAlbumIndex];
		    loadSongs(songs, controller.currAlbum.path);
		} else if (currMode==2){
		    controller.currSongIndex=(controller.currSongIndex+1)%songs.size();
		}
		break;
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
		} 
		break;

	    case '\t':
		currMode = (currMode+1)%3;
		break;
		
	}
    }
    worker.join();
    printf("\n\n\n\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

