#include "termigine.h"
#include <iostream>
#include <string>
#include <vector>
#include <print>
#include <fstream> // read all line of a file
#include <filesystem> // read all file of a dir
#include <typeinfo>


using namespace std;

Engine engine;
const string HOME = getenv("HOME");
static const int SCREEN_COLS = engine.getTerminalWidth();
static const int SCREEN_ROWS = engine.getTerminalHeight();

vector<string> TEMP_albums{
    HOME + "/Music/Amore_1982/",
    HOME + "/Music/Les_Failles_2019/",
    HOME + "/Music/Suite_bergamasque_1905/",
};

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

void drawCurrentSong(){

    string songName = "[SONG]";
    string albumName = "[ALBUM]";
    string songYear = "[YEAR]";
    string artistName = "[ARTIST]";
    print("\033[97m");
    engine.print(songName+" - "+albumName+" - "+songYear, 3, 2);
    engine.print("|| 0:00 / 0:00\n", 3, 3);
    print("\033[0m");
    engine.print("by "+artistName, SCREEN_COLS-artistName.length()-3, 2);
}


void drawAlbums(const vector<string> &albums){

    int rowStart = 6;
    int infoCount = 0; // decides which info to decorate style

    for (const auto &album : albums){
	/* display album art */
	string cmd ="chafa --format=kitty -s 7x7 " + album + "cover.jpg"; 
	print("\033[{};{}H", rowStart, SCREEN_COLS-35);
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
		engine.print("\033[97m\033[1m"+l+"\033[0m", SCREEN_COLS-26, rowStart);
	    } else {
		/* rest of the info is italic + mute */
		engine.print("\033[3m"+l+"\033[23m", SCREEN_COLS-26, rowStart);
	    }
	    rowStart++;
	    infoCount++;
	}
	infoCount=0; // reset 
	rowStart++;
    }
}

int main(){
    engine.setCanonicalAndCursor(0);
    engine.clearScreen();

    /* Preliminary check on screen size */
    if (SCREEN_ROWS<40 || SCREEN_COLS<94){
	printf("Program failed: terminal size too small for the UI. Need at least 94 columns and 40 rows, else it's GG\n");
	engine.setCanonicalAndCursor(1);
	return 0;
    }
    drawCurrentSong();
    engine.drawHorizontalSmoothLine(3, SCREEN_COLS, 5);
    engine.drawVerticalSmoothLine(6, SCREEN_ROWS, SCREEN_COLS-37);
    drawAlbums(TEMP_albums);

    drawSongsOfAlbum(TEMP_albums[1]);
    printf("\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

