## Soli

Hybrid music player TUI written from scratch. Uses [SDL2](https://github.com/libsdl-org/SDL/blob/SDL2/include/SDL_audio.h) for audio decoding, but no other non-standard libraries are used, everything else is written from the first principles (including terminal graphics).

Not only is it a great learning opportunity about the file system / file operations / terminal IO / OOP / multithreading / etc., it also serves as a showcase for my custom terminal graphics engine [termigine](https://github.com/randytylor69/terminal-graphics-engine).

&nbsp;

<div align="center">
<img height="300" src="./screenshots/newscreenshot_080.jpg" />
</div>

&nbsp;

## Planned Features

- [x] Play local `.wav` audio files
- [x] Pause & resume current playing song
- [x] Display current song length + progress
- [x] Display albums & cover art
- [x] Display all songs in an album
- [x] Highlighting selected elements (i.e. album / song)
- [x] Vim motion as controls
- [ ] Adding local directories as albums
- [ ] Searching YouTube results locally and download directly from the TUI
- [ ] Responsive layout

## Build

1. Soli uses SDL2 to play audio, [ensure it's intalled on your system](https://wiki.libsdl.org/SDL2/Installation).

2. Soli uses SQLite to store shadow copies of albums, [ensure it's installed too](https://www.sqlite.org/download.html).

3. Initially, Soli does not store any albums. To add an album to the SQLite database, first make sure the album directory is correctly configured, meaning it must have:

- A cover image named `cover.jpg`
- A `metadata.txt`
- Audio files ending in `.wav`

The `metadata.txt` file should include relevant information regarding the album: the album name, the artist name, and the year. For example:

```txt
Rubber Soul
The Beatles
1965
```

Then you can add the album to the database:

```shell
make && ./main.out <ABSOLUTE_PATH_TO_ALBUM_DIRECTORY>
```

4. Run the program:
```shell
make && ./main.out
```
