---
name: short
description: Turn the latest OBS recording into a YouTube Short (hook → animated breadboard schematic → footage → cover end card, music, standardized title/description), then archive it and record the published link in the repo. Use when the user says /short, "сделай шортс", "подготовь видео", or gives a YouTube link for a freshly uploaded video.
---

# /short

Talk to the user in Russian; title, description and repo text in English. Videos are never committed to git.

## Folders

| What | Where |
|---|---|
| New OBS recordings (unprocessed only) | `~/Movies/embedded-raw/` |
| Video being prepared / waiting for upload | `~/Movies/embedded/WNN-N-<slug>/` |
| Published (archive) | `~/Movies/embedded/published/WNN-N-<slug>/` |
| Music (YouTube Audio Library only) | `~/Music/shorts/` |
| Video log (numbering source of truth) | `docs/videos.md` |

Each video folder holds: `raw.mp4` (original OBS file), `schematic.mp4`, `cover.png`, `short.mp4` (final), `upload.txt` (title + description).
Moving the raw file out of `embedded-raw/` as soon as it is processed is what keeps recordings from getting mixed up.

## Part A: prepare the video

1. **Pick the file.** Newest file in `~/Movies/embedded-raw/` unless the user names one. Show name, time, duration (`ffprobe`).
   If there are several files, list them and ask which one(s); never guess between two takes.
2. **Find context.**
   - Current task: Chronix `search_tasks project=EMB status=in_progress` (the weekly one, not EMB-19), `get_task` → week `WNN`.
   - Project: `projects/NN-*` for that week (README line `Week: WNN`). Current step: the newest `steps/NN-*.cpp` / what `src/main.cpp` says.
   - Number N: rows in `docs/videos.md` with that week + 1. Slug: 2–4 words, kebab-case.
   - Create `~/Movies/embedded/WNN-N-<slug>/` and **move** the raw file there as `raw.mp4`.
3. **Look at the footage.** Contact sheet at 1 frame/s: `ffmpeg -i raw.mp4 -vf "fps=1,scale=360:-1,tile=4x3" <scratchpad>/raw%02d.jpg`. Measure audio (`volumedetect`).
   Decide footage `start`/`end` (cut OBS clicks, dead time, pop-ups; aim 15–45 s).
   **No hook by default** (`-`): the video starts with the schematic. He found a 2 s footage teaser before the schematic confusing (2026-11-27).
   Use a hook only if he asks for one.
4. **Wiring JSON for the schematic** (format: `tools/video/src/spec.ts`).
   - Use `projects/NN-*/steps/stepK.wiring.json` for the current step, or the newest one with a lower K if this step didn't change the circuit
     (check against `src/main.cpp` pins and the frames).
   - **None fits** → write it yourself from `src/main.cpp` (pins), the step's wiring table (`steps/*.html` / chat) and the frames;
     save as `steps/stepK.wiring.json`, render only the cover first and show it to him for a quick "схема верная?" before the full render.
   - Hole positions should match his real breadboard when known; if unknown, a clean plausible layout is fine (it's an illustration), say so.
   - Board not supported by the renderer yet (only `uno` today) or a part type missing → skip the schematic (use `tools/short.sh`, old flow),
     say what is missing so it can be added to `tools/video/src/parts.tsx`.
5. **Render schematic + cover:**
   `tools/video/render.sh <wiring.json> ~/Movies/embedded/WNN-N-<slug> "WNN #N" "<2–4 BIG WORDS>" <arduino|esp32|stm32|rpi>`
   Cover words: 2–4 words, uppercase-friendly, with → where it fits (e.g. `POT → LED`).
6. **Sound.** Default `mute` + a random track from `~/Music/shorts/`, different from the last row's Music in `docs/videos.md`.
   Meaningful sound (buzzer, voice) → `norm` (+ music underneath). Folder empty → no music, remind how to add tracks (`docs/recording.md`).
   Credits: if `<track>.txt` exists next to the track, append its text under `Music:` in the description; if it says a per-video
   license is required, tell him to get it BEFORE uploading.
7. **Assemble:**
   `tools/assemble.sh raw.mp4 short.mp4 <start> <end> - schematic.mp4 cover.png <music|-> <mute|norm>` (run inside the folder).
   Result: schematic (ends on the finished state, LED at full) ⇢ footage ⇢ cover card, with crossfades.
8. **Privacy check of `short.mp4`** (always, never skip). Sheets at 1 frame/s: `fps=1,scale=360:-1,tile=4x3`, and zoom on the screen part
   (`crop=1080:820:0:1100,fps=1,scale=540:-1,tile=2x3`) when text is small. Look for:
   - **Screen:** notifications/pop-ups, menus, other apps or tabs, file paths/usernames, emails, phone numbers, tokens/passwords/API keys, Wi-Fi names, chats.
   - **Camera:** faces/people, documents, parcels with name/address, bank cards, other screens, reflections, window views.
   - **Policy:** nothing YouTube would flag. Drone/robot hardware is fine.
   - **Sound:** with `norm`, warn that voices are NOT checked — he must listen.
   Report ✅ nothing found, or ⚠️ with timestamps and a fix (re-assemble with a tighter range). It's a 1 fps sample: say that split-second things can be missed.
9. **Write `upload.txt`** (title + description, templates below) and show everything in ONE message:
   folder path, duration and structure (schematic / footage / card), what was cut, music, privacy result, title, description,
   and that `cover.png` is in the folder. Ask only: "ок или поправить?" Check title/description contain nothing personal.

## Title template

```
Embedded from zero · WNN #N: <What is shown>
```
- `<What is shown>`: 2–6 words, concrete, sentence case. E.g. `PWM on an oscilloscope`, `Potentiometer → ADC → Teleplot`. Whole title ≤ 70 chars.

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

1. Append to `docs/videos.md`: `| WNN | N | <What is shown> | [NN-name](../projects/NN-name/) | <link> | <music file name or —> |`.
2. Project README, section `## Progress videos` (create if missing): `- <What is shown>: <one sentence>. 🎥 <link>`
3. Final demo of a project → also the root `README.md` table (Video column).
4. Move `~/Movies/embedded/WNN-N-<slug>/` to `~/Movies/embedded/published/` and write the link into its `upload.txt`.
5. Disk is nearly full: if `df -h ~` shows < 10 GB free, suggest deleting `raw.mp4` from old published folders (ask first).
6. Do not commit or push unless asked.
