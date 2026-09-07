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

&nbsp;

## Controls

Soli uses [Vim Motions](https://vimdoc.sourceforge.net/htmldoc/motion.html) to navigate:

| Key | Description |
| --- | --- |
| `j` / `k` | Move up / down |
| `[ENTER]` | Play currently selected song |
| `[SPACE]` | Pause / Resume currently selected song |
| `q` | Quit program | 

&nbsp;

## Build

1. Soli uses SDL2 to play audio, [ensure it's intalled on your system](https://wiki.libsdl.org/SDL2/Installation). It also uses SQLite to store shadow copies of albums, [ensure it's installed too](https://www.sqlite.org/download.html).

2. Initially, Soli does not store any albums, they have to be manually appened to a database named `db.sqlite3`, which needs to be created first:

```shell
touch db.sqlite3
```

Then, make sure the album directory you wish to add to Soli is correctly configured, meaning it must have:

- A cover image named `cover.jpg`
- A `metadata.txt`
- Audio files ending in `.wav`

The `metadata.txt` file should include relevant information regarding the album: the album name, the artist name, and the year (in this exact order). For example:

```txt
Rubber Soul
The Beatles
1965
```

Then you can add the album to the database:

```shell
make 
./main.out --add <ABSOLUTE_PATH_TO_ALBUM_DIRECTORY>
```

Or delete an album from the database:

```shell
./main.out --delete <ABSOLUTE_PATH_TO_ALBUM_DIRECTORY>
```

Or display all albums you currently have:

```shell
./main.out --show
```

To run the program, simply call the executable:

```shell
make && ./main.out
```
