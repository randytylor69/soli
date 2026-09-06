#ifndef SOLI_H_
#define SOLI_H_
#include <string>
#include "termigine.h"
#include <vector>
#include <optional>
#include <SDL2/SDL.h>
using namespace std;

struct Album {
    string path, name, artist, year, art;
    Album()=default;
    Album(string pathi, string namei, string artisti, string yeari, string arti)
	:path(pathi), name(namei), artist(artisti), year(yeari), art(arti) {}
};

struct Song {
    string path, name;
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

    SDL_AudioDeviceID currAudioDevice;
    int isPaused = 1; // is current playing device paused, non-zero to pause, 0 to unpause
    int currAlbumIndex = 0;
    int currSongIndex = 0;
    Controller() {}
};
void drawSongsOfAlbum(string album_path, vector<Song> songs, Controller controller, int currMode);
void drawCurrentSong(Controller controller);
void drawAlbums(vector<Album> &albums, Controller controller, int currMode);
bool checkScreenSize();
void loadAlbums(vector<Album> &albums, vector<string> album_paths);
void loadSongs(vector<Song> &songs, string album_path);
/* VIM MOTION METHODS */
void toggleMoveDown(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums);
void toggleMoveUp(Controller &controller, char currMode, vector<Song> &songs, vector<Album> &albums);
void toggleModeChange(Controller &controller, char &currMode, const vector<Song> &songs);
void playSelectedSong(Controller &controller, int currMode, SDL_AudioSpec spec, Uint8 * audio_buf, Uint32 audio_len);
void pauseSelectedSong(Controller &controller, int currMode);
#endif
