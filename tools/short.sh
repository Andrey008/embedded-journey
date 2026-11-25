#!/usr/bin/env bash
# Turn a raw OBS recording into a YouTube-ready Short.
#
# Usage: tools/short.sh <input> <output> [start_sec] [end_sec] [audio] [music]
#   start_sec  where to start (default 0)
#   end_sec    where to stop  (default: end of file)
#   audio      "norm" = normalize loudness (default), "mute" = drop original sound, "keep" = as is
#   music      optional track (mp3/wav/m4a). Looped if shorter, faded in/out.
#              With audio=mute it's the only sound; otherwise it plays quietly underneath.
set -euo pipefail

in="$1"; out="$2"; ss="${3:-0}"; to="${4:-}"; audio="${5:-norm}"; music="${6:-}"

trim=(-ss "$ss")
[[ -n "$to" ]] && trim+=(-to "$to")

# Final duration, needed for the music fade-out
if [[ -n "$to" ]]; then
  dur=$(echo "$to - $ss" | bc -l)
else
  full=$(ffprobe -v error -show_entries format=duration -of csv=p=0 "$in")
  dur=$(echo "$full - $ss" | bc -l)
fi
fade_at=$(echo "$dur - 2" | bc -l)

vf="scale=1080:1920:force_original_aspect_ratio=decrease,pad=1080:1920:(ow-iw)/2:(oh-ih)/2,fps=30"
loud="loudnorm=I=-14:TP=-1.5:LRA=11"   # YouTube loudness target (-14 LUFS)

case "$audio" in
  norm) orig="$loud" ;;
  keep) orig="anull" ;;
  mute) orig="" ;;
  *) echo "audio must be norm|mute|keep" >&2; exit 1 ;;
esac

inputs=("${trim[@]}" -i "$in")
if [[ -n "$music" ]]; then
  inputs+=(-stream_loop -1 -i "$music")
  mus="[1:a]atrim=0:${dur},asetpts=N/SR/TB,afade=t=in:d=1,afade=t=out:st=${fade_at}:d=2"
  if [[ -z "$orig" ]]; then
    fc="[0:v]${vf}[v];${mus},${loud}[a]"                        # music only
  else
    fc="[0:v]${vf}[v];[0:a]${orig}[o];${mus},${loud},volume=0.25[m];[o][m]amix=inputs=2:duration=first:normalize=0[a]"
  fi
else
  if [[ -z "$orig" ]]; then
    fc="[0:v]${vf}[v];[0:a]volume=0[a]"
  else
    fc="[0:v]${vf}[v];[0:a]${orig}[a]"
  fi
fi

mkdir -p "$(dirname "$out")"
# H.264 + AAC, no metadata (dates, device names), moov atom at the start so YouTube starts processing faster.
ffmpeg -hide_banner -loglevel error -y "${inputs[@]}" \
  -filter_complex "$fc" -map "[v]" -map "[a]" -t "$dur" \
  -c:v libx264 -preset medium -crf 20 -pix_fmt yuv420p \
  -c:a aac -b:a 160k -map_metadata -1 -movflags +faststart "$out"

ffprobe -v error -show_entries format=duration -of csv=p=0 "$out" | xargs printf "OK %s (%.1f s)\n" "$out"
