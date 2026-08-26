## Soli

Hybrid usic player TUI written from scratch, a showcase for my custom terminal graphics engine [termigine](https://github.com/randytylor69/terminal-graphics-engine).

| ![Screenshot](./screenshots/newscreenshot_056.jpg)|
| --- |
| Directly `.mp3` playback inside the terminal |

The `.mp3` playback feature is supported entirely by [miniaudio.h](https://miniaud.io/), since **soli** is built to explore the user & terminal & filesystem interactions, I dismissed the effort to wrte an audio encoder from scratch. It's also why this project has ~100,000 lines and C as its primary language, all from *miniaudio*.

&nbsp;

## Planned Features

- Listening to local audio files
- Adding directories as albums
- Searching YouTube results locally and download directly from the TUI


