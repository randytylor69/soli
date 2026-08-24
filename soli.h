#ifndef SOLI_H_
#define SOLI_H_
#include <string>
#include "termigine.h"
#include <vector>
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
    int currAlbumIndex = 0;
    int currSongIndex = 0;
    Controller() {}
};
void drawSongsOfAlbum(string album_path, vector<Song> songs, Controller controller, int currMode);
void drawCurrentSong(string song, string album, string year, string artist);
void drawAlbums(vector<Album> &albums, Controller controller, int currMode);
bool checkScreenSize();

#endif
