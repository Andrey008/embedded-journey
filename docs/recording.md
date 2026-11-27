# Recording a Short

## Before recording (1 min)
1. Phone in the holder above the breadboard, **DroidCam OBS** app open, screen on (plug in the charger).
2. Mac: **Do Not Disturb** on, so notifications don't pop up over the window.
3. Arduino connected, firmware uploaded, Serial Monitor (Terminal) **or** Teleplot running.
   Only one of them can hold the port.
4. OBS: choose the scene `Short: Serial` or `Short: Teleplot`.
   Check the preview: camera is live (not frozen), board is in the center, the bottom shows data.

## Recording
5. **Start Recording** (not *Start Streaming*).
6. Show the thing in 15–45 s. Pauses at the start and end are fine: `/short` trims them.
7. **Stop Recording**. The file lands in `~/Movies/embedded-raw/`.

## Publishing
8. In Claude Code: `/short`. The recording moves to `~/Movies/embedded/WNN-N-name/` and you get there:
   `short.mp4` (hook → animated schematic → footage → cover), `cover.png`, and `upload.txt` with the title and description.
9. YouTube Studio → Create → Upload → `short.mp4` → paste title + description → thumbnail `cover.png` (or pick the last frame in the YouTube app) → Visibility: Unlisted (or Public).
10. Send the link to Claude: it goes to `docs/videos.md` and the project README, the folder moves to `~/Movies/embedded/published/`.

## Music
Only tracks from the **YouTube Audio Library** are safe from copyright claims.
1. YouTube Studio → **Audio Library** (left menu).
2. Filter **Attribution: not required**. Calm genres work well: Ambient, Electronic, Cinematic.
3. Download 5–10 tracks into `~/Music/shorts/`.

If a track needs credit, put a text file with the **same name** next to it (`track.mp3` → `track.txt`)
with the attribution line. `/short` adds it to the description. Tracks that need a per-video license
(e.g. Declan DP from SoundCloud) → write that in the `.txt`, and get the license before each upload.

`/short` picks one at random and plays it instead of the room noise (or quietly under real sound).
Don't use tracks from TikTok, Spotify or random "no copyright" channels: those are what get claimed.

## If something goes wrong
| Symptom | Fix |
|---|---|
| "Starting the output failed" | You pressed *Start Streaming*. Use *Start Recording*. |
| Camera frozen / black | Phone screen turned off or DroidCam went to background. Reopen the app. |
| Bottom area black | OBS lost screen-capture permission or the window was closed. Re-pick the window in the source. |
| Video is small / blurry | Settings → Video → Output (Scaled) Resolution must be `1080x1920`. |
| "port busy" in Terminal | VS Code Serial Monitor or Teleplot still holds the port. Close it. |
