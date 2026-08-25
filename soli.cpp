#include "soli.h"
#include "termigine.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

extern Engine engine;
extern vector<Album> albums;

static const int SCREEN_ROWS = engine.getTerminalHeight();
static const int SCREEN_COLS = engine.getTerminalWidth();

/* __________________ METHODS _________________ */

void drawSongsOfAlbum(string album_path, vector<Song> songs, Controller controller, int currMode){
    int rowCount = 6;
    int songCount = 0;
    for (const Song &song : songs){
	songCount++;
	string songOrder = to_string(songCount) + (
	    songCount < 10 ? "... " : ".. " // format order	
	);
	/* currently selected song is bold + italic + colored */
	engine.print(
	    controller.currSongIndex == songCount - 1 && currMode==2 ?
	    songOrder+"\033[38;5;209m\033[1m\033[3m"+song.name+"\033[0m":
	    songOrder+"\033[97m"+song.name+"\033[0m",
	    3, rowCount);
	rowCount++;
    }
    /* in case of empty album */
    if (songCount==0) {
	engine.print("This album is currently empty ~", 3, rowCount);
    }
}

void drawCurrentSong(Controller controller){
    
    if (!controller.currPlayingSong.has_value()){
	engine.print("\033[3mEmpty Track... select a song and press [ENTER] to play",3, 2);
	
    } else {
	engine.print("\033[97m"+
	    controller.currPlayingSong->name+" - "+
	    controller.currPlayingAlbum->name+" - "+
	    controller.currPlayingAlbum->year,
	    3, 2);
	engine.print("|| 0:00 / 0:00\033[0m\n", 3, 3);
	engine.print("by "+
	    controller.currPlayingAlbum->artist, 
	    SCREEN_COLS-controller.currPlayingAlbum->artist.length()-3, 2);
    }
}


void drawAlbums(vector<Album> &albums, Controller controller, int currMode){

    int rowStart = 6;
    int infoCount = 0; // decides which info to decorate style

    for (const Album &album : albums){

	/* display album art */
	print("\033[{};{}H", rowStart, SCREEN_COLS-35); //mv cursor
	fflush(stdout);
	print("{}", album.art);
	engine.setCanonicalAndCursor(0);

	/* display album info */
	string nameOutput = album.name;
	string artistOutput = album.artist;
	string yearOutput = album.year;

	if (nameOutput.length() > 20) {
	    nameOutput = nameOutput.substr(0, 19) + "...";
	}
	if (artistOutput.length() > 20) {
	    artistOutput = artistOutput.substr(0, 19) + "...";
	}
	/* print album name */
	engine.print(
	    controller.currAlbum.path == album.path && currMode==1 ?
	    "\033[38;5;209m\033[1m> "+nameOutput+"\033[0m" :
	    "\033[97m\033[1m"+nameOutput+"\033[0m",

	    SCREEN_COLS-26, rowStart++);

	/* print album artist */
	engine.print(
	    controller.currAlbum.path == album.path && currMode==1 ?
	    "\033[3m\033[38;5;209m"+artistOutput+"\033[23m" :
	    "\033[3m"+artistOutput+"\033[23m",
	    SCREEN_COLS-26, rowStart++);

	/* print album year */
	engine.print(
	    "\033[3m"+yearOutput+"\033[23m\033[0m",
	    SCREEN_COLS-26, rowStart++);

	rowStart++;

	
    }
}

bool checkScreenSize(){

    if (SCREEN_ROWS<40 || SCREEN_COLS<94){
	printf("Program failed: terminal size too small for the UI. Need at least 94 columns and 40 rows, else it's GG\n");
	engine.setCanonicalAndCursor(1);
	return false;
    }
    return true;
};

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
