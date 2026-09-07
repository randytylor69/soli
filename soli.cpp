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

string getFormattedTime(int lenTotal){
    // lenTotal is in seconds.
    string formatted_result = "";
    int seconds = lenTotal % 60;
    int minutes = (lenTotal - seconds) / 60;

    formatted_result += to_string(minutes);
    formatted_result += ":";
    if (seconds < 10) formatted_result += "0";
    formatted_result += to_string(seconds);
    return formatted_result;


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
	/* print song duration + progress */
	print("\033[{};{}H{} ", 3, 3, controller.isPaused? "[ || ]" : "[ >> ]");
	fflush(stdout);
	string formatted_currSongProgress = "";
	string formatted_currSongLength = "";
	
	print("{} / {}\033[0m\n", getFormattedTime(controller.currSongProgress), getFormattedTime(controller.currSongLength));

	/* Print artist */
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

void loadSongs(vector<Song> &songs, Controller &controller){
    songs = {}; 
    string album_path = controller.currAlbum.path;

    for (auto &file : filesystem::directory_iterator(album_path)){
	string fname = file.path().filename().string();
	if (fname.substr(fname.length()-4, 4)!=".wav") continue;
	
	fname = fname.substr(0, fname.length()-4); // rm extension
	if (fname.length() > 45) { // abbreviate if name too long
	    fname = fname.substr(0, 44) + "...";
	}

	songs.push_back(Song(file.path(), fname));
	controller.currAlbum.size++;
    }
}


/* --------------- VIM MOTION METHOD ----------------------- */


void toggleMoveDown(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums){

    if (currMode==1){
	controller.currAlbumIndex=(controller.currAlbumIndex+1)%albums.size();
	controller.currAlbum = albums[controller.currAlbumIndex];
	loadSongs(songs, controller);
    } else if (currMode==2){
	controller.currSongIndex=(controller.currSongIndex+1)%songs.size();
	controller.currSong = songs[controller.currSongIndex];
    }
}

void toggleMoveUp(Controller &controller, char currMode, vector<Song> &songs, vector<Album>&albums){

    if (currMode==1){
	controller.currAlbumIndex=
	    controller.currAlbumIndex==0?
	    albums.size()-1 : controller.currAlbumIndex-1;
	controller.currAlbum = albums[controller.currAlbumIndex];
	loadSongs(songs, controller);
    } else if (currMode==2){
	controller.currSongIndex=
	    controller.currSongIndex==0?
	    songs.size()-1: controller.currSongIndex-1;
	controller.currSong = songs[controller.currSongIndex];
    } 
}

void toggleModeChange(Controller &controller, char &currMode, const vector<Song> &songs){

    currMode = (currMode+1)%3;
    if (currMode == 2){
	controller.currSong = songs[0];
	controller.currSongIndex = 0; // reset to the first song
    }
}

void playSelectedSong(Controller &controller, int currMode){
    SDL_AudioSpec spec = controller.spec;
    Uint8 * audio_buf = controller.audio_buf;
    Uint32 audio_len = controller.audio_len;

    if (currMode == 2){
	/* if a song is already playing, remove it */
	if (controller.currAudioDevice > 0){
	    SDL_PauseAudioDevice(controller.currAudioDevice, 1);
	    SDL_ClearQueuedAudio(controller.currAudioDevice);
	    SDL_CloseAudioDevice(controller.currAudioDevice);
	    controller.currAudioDevice = 0;
	    controller.currSongProgress = 0;
	}
	/* push current selected to playing */
	controller.currPlayingAlbum = controller.currAlbum;
	controller.currPlayingSong = controller.currSong;

	/* load .wav */    
	SDL_AudioSpec *returnSpec = SDL_LoadWAV(
		controller.currSong.path.c_str(), 
		&spec, &audio_buf, &audio_len
	);
	/* get song duration */
	int sampleSize = SDL_AUDIO_BITSIZE(spec.format) / 8;
	int sampleCount = audio_len / sampleSize;
	int sampleLength = 0;
	if (spec.channels){
	    sampleLength = sampleCount / spec.channels;
	} else sampleLength = sampleCount;
	controller.currSongLength = (double)sampleLength / (double)spec.freq;

	/* opening .wav */
	controller.currAudioDevice = SDL_OpenAudioDevice(
		NULL,0,returnSpec,NULL,0);

	/* queueing .wav */
	SDL_QueueAudio(controller.currAudioDevice, audio_buf, audio_len);
	controller.isPaused = 0;
	SDL_PauseAudioDevice(controller.currAudioDevice, controller.isPaused); // unpause
    }
}

void pauseSelectedSong(Controller &controller, int currMode){
    if (currMode==2){
	controller.isPaused = !controller.isPaused;
	SDL_PauseAudioDevice(controller.currAudioDevice, controller.isPaused);
    }
}


void checkSongProgress(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums){
    if (controller.currAudioDevice > 0 && !controller.isPaused){
	/* NOTE: isPaused returns FALSE if UNPAUSED */
	Uint32 audioSize = SDL_GetQueuedAudioSize(controller.currAudioDevice);
	if (audioSize == 0){
	    /* song has finished */
	   controller.currSongLength = 0; 
	   controller.currSongProgress = 0; 
	   controller.isPaused = 1; 
	   /* play next song */
	   if (controller.currSongIndex < controller.currAlbum.size){
	       toggleMoveDown(controller, currMode, songs, albums);
	       playSelectedSong(controller, currMode);
	   }
	}
    }
}
