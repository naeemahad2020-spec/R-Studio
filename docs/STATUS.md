# Status
| Stage | Scope | State |
|---|---|---|
| 1 | Project, UI shell, installer script | Source written, NOT compiled/tested |
| 2 (part 1) | Scenes/sources saved in SQLite; screen preview; desktop recording to MKV via ffmpeg | Source written, NOT compiled/tested |
| 2 (remaining) | Real scene compositing, window/camera sources, MP4 remux | Not started |
| 3 | Audio, encoding presets, media playback | Not started |
| 4 | One real streaming destination | Not started |
| 5-10 | Everything else | Not started |

Known limits of Stage 2 part 1: no audio in recordings; recording captures the
whole desktop, not the selected scene; ffmpeg.exe must be supplied separately
(LGPL build recommended; libx264 is GPL).
