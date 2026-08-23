#include "termigine.h"
#include <iostream>
#include <string>
#include <vector>
#include <print>
#include <fstream>

#define SCREEN_COLS 85
#define SCREEN_ROWS 40

using namespace std;

Engine engine;
const string HOME = getenv("HOME");

vector<string> TEMP_albums{
    HOME + "/Music/Amore_1982/",
    HOME + "/Music/Les_Failles_2019/",
    HOME + "/Music/Suite_bergamasque_1905/",
};

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

void drawHorizontalSeparator(){
    for (int i=3; i<SCREEN_COLS-1; i++){
	engine.print("─", i, 5);
    }
}

void drawVerticalSeparator(){
    for (int i=6; i<SCREEN_ROWS; i++){
	engine.print("│", SCREEN_COLS/2+6, i);
    }
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
	    if (infoCount==0){
		/* album name is bold + bright white */
		if (l.length() > 20) {
		    l = l.substr(0, 19) + "...";
		}
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
    if (engine.getTerminalHeight()<40 || engine.getTerminalWidth()<85){
	printf("Program failed: terminal size too small for the UI. Need at least 85 columns and 40 rows, else it's GG\n");
	engine.setCanonicalAndCursor(1);
	return 0;
    }
    drawCurrentSong();
    drawHorizontalSeparator();
    drawVerticalSeparator();
    drawAlbums(TEMP_albums);

    printf("\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

