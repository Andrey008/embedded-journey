---
name: short
description: Turn the latest OBS recording into a YouTube Short with a standardized title and description, then record the published link in the repo. Use when the user says /short, "сделай шортс", "подготовь видео", or gives a YouTube link for a freshly uploaded video.
---

# /short

Raw recordings: `~/Movies/embedded-raw/` (OBS, already 1080x1920 vertical: camera on top, Serial/Teleplot below).
Ready files: `~/Movies/embedded-ready/` (outside git: videos are never committed).
Video log: `docs/videos.md` (source of truth for numbering).
Talk to the user in Russian; title, description and repo text in English.

## Part A: prepare the video

1. **Pick the file.** Newest file in `~/Movies/embedded-raw/` unless the user names one. Show its name, time and duration (`ffprobe`).
2. **Look at it.** Extract ~6 frames evenly (`ffmpeg -ss T -i f -frames:v 1 -vf scale=360:-1 <scratchpad>/fN.jpg`) and view them. Measure audio with `volumedetect`.
3. **Find context.**
   - Current task: Chronix `search_tasks project=EMB status=in_progress` (the weekly one, not EMB-19), then `get_task` → week `WNN`.
   - Project: the `projects/NN-*` folder for that week (README line `Week: WNN`).
   - Number: rows in `docs/videos.md` with that week + 1.
4. **Decide trimming.** From the frames, suggest start/end seconds that cut dead time (hands reaching for OBS, empty frames). Shorts limit is 3 min; aim for 15–45 s.
5. **Decide sound.** Music tracks live in `~/Music/shorts/` (only YouTube Audio Library tracks, safe from Content ID).
   - Default: `mute` + a random track from that folder, different from the last row's Music in `docs/videos.md`. The room mic is mostly noise.
   - If the user says the video has meaningful sound (buzzer, voice): `norm` + music (plays quietly underneath), or `norm` without music if they ask.
   - Folder empty or missing → `norm`, no music, and remind them how to add tracks (see `docs/recording.md`).
   - **Credits:** if `<track>.txt` exists next to the track, append its text to the end of the description under `Music:`.
     If it says a per-video license is required (e.g. Declan DP → license.declandp.info), tell him to generate the
     license for this video and paste it into the description BEFORE uploading. No `.txt` → assume no attribution needed, but say so.
6. **Render:** `tools/short.sh "<raw>" ~/Movies/embedded-ready/WNN-N-<slug>.mp4 <start> <end> <audio> [music]`
7. **Privacy check of the rendered file** (always, never skip). Make contact sheets at 1 frame/s:
   `ffmpeg -i <ready> -vf "fps=1,scale=360:-1,tile=4x3" <scratchpad>/sheet%02d.jpg` and look at every sheet.
   Also zoom on the bottom (screen) half if text is small: `crop=1080:820:0:1100,fps=1,scale=540:-1,tile=2x3`.
   Look for:
   - **Screen:** notifications/pop-ups, other apps or browser tabs, file paths or usernames, emails, phone numbers,
     tokens/passwords/API keys, Wi-Fi names, chat messages.
   - **Camera:** faces or people in the background, documents, shipping labels or parcels with name/address,
     bank cards, other screens (phone, monitor), reflections, anything showing where he lives (window view, street).
   - **Legal/policy:** nothing dangerous or that YouTube would flag (weapons, drugs, etc.). Drone/robot hardware itself is fine.
   - **Sound:** if `norm`/`keep` was used, warn that speech/background voices are NOT checked (no transcription) — he must listen.
   Report as: ✅ nothing found, or ⚠️ list with timestamps (second N) and a fix (cut the range / blur / re-record).
   Be honest that this is a 1-frame-per-second sample: something shown for a split second can be missed.
8. **Write title + description** (template below) and show everything in ONE message:
   file path, duration, what was trimmed, music track, privacy check result, title, description. Ask only: "ок или поправить?"
   Also check the title/description themselves contain nothing personal (no name, city, email).

## Title template

```
Embedded from zero · WNN #N: <What is shown>
```
- `<What is shown>`: 2–6 words, concrete, capitalized like a sentence. E.g. `PWM on an oscilloscope`, `Potentiometer → ADC → Teleplot`.
- Whole title ≤ 70 characters.

## Description template

```
<1–2 sentences: what happens on screen, plain words.>

Hardware: <boards/parts visible, comma-separated>
Code & notes: https://github.com/Andrey008/embedded-journey/tree/main/projects/<NN-name>

Week NN of my 16-week embedded journey: Arduino → ESP32 → STM32 → a small ground robot.

#arduino #embedded #electronics #shorts
```
Replace `#arduino` with `#esp32` / `#stm32` / `#raspberrypi` to match the platform.

## Part B: after upload (user sends the YouTube link)

1. Append a row to `docs/videos.md`: `| WNN | N | <What is shown> | [NN-name](../projects/NN-name/) | <link> | <music file name or —> |`.
2. Add to the project README under `## Progress videos` (create the section if missing):
   `- <What is shown>: <one sentence>. 🎥 <link>`
3. If it is the final demo of the project, also put the link in the root `README.md` table (Video column).
4. Offer to move the raw file to Trash (`~/Movies/embedded-raw/`) — ask first, never delete silently.
5. Do not commit or push unless asked.
