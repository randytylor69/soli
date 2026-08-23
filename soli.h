#ifndef SOLI_H_
#define SOLI_H_
#include <string>
#include "termigine.h"
#include <vector>
using namespace std;

void drawSongsOfAlbum(string albumDir);
void drawCurrentSong(string song, string album, string year, string artist);
void drawAlbums(const vector<string> &albums);

#endif
