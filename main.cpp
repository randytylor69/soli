#include "termigine.h"
#include "soli.h"
#include <iostream>
#include <string>
#include <vector>


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


int main(){
    engine.setCanonicalAndCursor(0);
    engine.clearScreen();

    /* Preliminary check on screen size */
    if (SCREEN_ROWS<40 || SCREEN_COLS<94){
	printf("Program failed: terminal size too small for the UI. Need at least 94 columns and 40 rows, else it's GG\n");
	engine.setCanonicalAndCursor(1);
	return 0;
    }
    drawCurrentSong("[SONG]", "[ALBUM]", "[YEAR]", "[ARTIST]");
    engine.drawHorizontalSmoothLine(3, SCREEN_COLS, 5);
    engine.drawVerticalSmoothLine(6, SCREEN_ROWS, SCREEN_COLS-37);
    drawAlbums(TEMP_albums);

    drawSongsOfAlbum(TEMP_albums[1]);
    printf("\n\n\n\n\n\n");
    engine.setCanonicalAndCursor(1);
    return 0;
}

