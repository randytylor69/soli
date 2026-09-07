#ifndef SOLI_H_
#define SOLI_H_
#include <string>
#include "termigine.h"
#include <vector>
#include <optional>
#include <sqlite3.h>
#include <stdio.h>
#include <SDL2/SDL.h>
using namespace std;

struct Album {
    string path, name, artist, year, art;
    int size = 0;
    Album()=default;
    Album(string pathi, string namei, string artisti, string yeari, string arti)
	:path(pathi), name(namei), artist(artisti), year(yeari), art(arti) {}
};

struct Song {
    string path, name;
    Song *nextSong; // for queue

    Song()=default;
    Song(string pathi, string namei)
	:path(pathi), name(namei){}
};

struct Controller {
    /* manages the global states: current album / song / etc. */
    Album currAlbum; 
    Song currSong;   
    
    optional<Album> currPlayingAlbum;
    optional<Song> currPlayingSong;
    int currAlbumIndex = 0;
    int currSongIndex = 0;

    /* SDL + song properties */
    SDL_AudioDeviceID currAudioDevice;
    SDL_AudioSpec spec;
    Uint8 * audio_buf;
    Uint32  audio_len;

    int isPaused = 1; // non-zero to pause, 0 to unpause
    int currSongLength = 0; // seconds
    int currSongProgress = 0; // seconds

    Controller() {}
};

struct PlayQueue{
    Song *head = NULL;
    Song *tail = NULL;
    
    PlayQueue() = default;
    void enqueue(Song *song);
    Song dequeue();
    void emptySelf();
};
void drawSongsOfAlbum(string album_path, vector<Song> songs, Controller controller, int currMode);
void drawCurrentSong(Controller controller);
void drawAlbums(vector<Album> &albums, Controller controller, int currMode);
bool checkScreenSize();
void loadAlbums(vector<Album> &albums, vector<string> album_paths);
void loadSongs(vector<Song> &songs, Controller &controller);

/* VIM MOTION METHODS */
void toggleMoveDown(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums);
void toggleMoveUp(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums);
void toggleModeChange(Controller &controller, char &currMode, const vector<Song> &songs);
void playSelectedSong(Controller &controller, int currMode);
void pauseSelectedSong(Controller &controller, int currMode);
void checkSongProgress(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums);

/* SQLite methods */

void openDB(const char* filename, sqlite3*& db);
void createTable(sqlite3* database);
void insertData(sqlite3 *db, string path);
void deleteData(sqlite3 *db, string path);
void readTable(sqlite3 *db, vector<string> &album_paths);
void readCMD(int argc, char* argv[], Engine engine, sqlite3* db, vector<string> &album_paths);
#endif
