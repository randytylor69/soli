#include "soli.h"
#include "termigine.h"
#include <filesystem>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;
extern Engine engine;

void drawSongsOfAlbum(string albumDir){
    int rowCount = 6;
    int songCount = 0;
    for (auto &file : filesystem::directory_iterator(albumDir)){
	/* trim & adjust file name */
	string fname = file.path().filename().string();
	if (fname.substr(fname.length()-4, 4) != ".mp3") continue;
	songCount++;
	fname = fname.substr(0, fname.length()-4); // rm extension
	if (fname.length() > 45) {
	    fname = fname.substr(0, 44) + "...";
	}
	string songOrder = to_string(songCount) + (
	    songCount < 10 ? "... " : ".. "	
	);
	engine.print(
	    songOrder+"\033[97m"+fname+"\033[0m",
	    3, rowCount);
	rowCount++;
    }
    /* in case of empty album */
    if (songCount==0) {
	engine.print("This album is currently empty ~", 3, rowCount);
    }
}

void drawCurrentSong(string song, string album, string year, string artist){

    engine.print("\033[97m"+song+" - "+album+" - "+year, 3, 2);
    engine.print("|| 0:00 / 0:00\033[0m\n", 3, 3);
    engine.print("by "+artist, engine.getTerminalWidth()-artist.length()-3, 2);
}


void drawAlbums(const vector<string> &albums){

    int rowStart = 6;
    int infoCount = 0; // decides which info to decorate style

    for (const auto &album : albums){
	/* display album art */
	string cmd ="chafa --format=kitty -s 7x7 " + album + "cover.jpg"; 
	print("\033[{};{}H", rowStart, engine.getTerminalWidth()-35);
	fflush(stdout);
	system(cmd.c_str());

	/* display album info */
	ifstream f(album+"metadata.txt");
	string l;
	while(getline(f, l)){
	    if (l.length() > 20) {
		l = l.substr(0, 19) + "...";
	    }
	    if (infoCount==0){
		/* album name is bold + bright white */
		engine.print("\033[97m\033[1m"+l+"\033[0m", engine.getTerminalWidth()-26, rowStart);
	    } else {
		/* rest of the info is italic + mute */
		engine.print("\033[3m"+l+"\033[23m", engine.getTerminalWidth()-26, rowStart);
	    }
	    rowStart++;
	    infoCount++;
	}
	infoCount=0; // reset 
	rowStart++;
    }
}
