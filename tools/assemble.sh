#!/usr/bin/env bash
# Build the final Short: [hook] → schematic intro ⇢ footage ⇢ cover end card (⇢ = crossfade).
#
# Usage: tools/assemble.sh <raw> <out.mp4> <start> <end> <hook_start|-> <schematic.mp4> <cover.png> [music|-] [audio]
#   start/end    footage range in seconds
#   hook_start   where an optional 2-second teaser of the footage starts ("-" = no hook, the default choice)
#   music        track, looped and faded over the whole video ("-" = none)
#   audio        "mute" (default) = drop room sound, "norm" = keep footage sound, normalized
set -euo pipefail

raw="$1"; out="$2"; ss="$3"; to="$4"; hook="$5"; sch="$6"; cover="$7"; music="${8:--}"; audio="${9:-mute}"
HOOK=2; CARD=1.6; XF1=0.5; XF2=0.4   # crossfades: schematic→footage, footage→card
FREEZE=1.2                             # hold the last footage frame so the ending can be seen
dur() { ffprobe -v error -show_entries format=duration -of csv=p=0 "$1"; }

main=$(echo "$to - $ss + $FREEZE" | bc -l)
schd=$(dur "$sch")
hookd=0; [[ "$hook" != "-" ]] && hookd=$HOOK
total=$(echo "$hookd + $schd + $main + $CARD - $XF1 - $XF2" | bc -l)
fade_at=$(echo "$total - 2" | bc -l)

V="scale=1080:1920:force_original_aspect_ratio=decrease,pad=1080:1920:(ow-iw)/2:(oh-ih)/2,fps=30,setsar=1,format=yuv420p,settb=AVTB"
A="aformat=sample_rates=48000:channel_layouts=stereo"
LOUD="loudnorm=I=-14:TP=-1.5:LRA=11"
silence() { echo "aevalsrc=0|0:d=$1:s=48000,$A"; }

inputs=(-i "$raw" -i "$sch" -loop 1 -t "$CARD" -i "$cover")
[[ "$music" != "-" ]] && inputs+=(-stream_loop -1 -i "$music")

# ---- video ----
fc="[1:v]$V,setpts=PTS-STARTPTS[s];"
fc+="[0:v]trim=start=$ss:end=$to,setpts=PTS-STARTPTS,$V,tpad=stop_mode=clone:stop_duration=$FREEZE[m];"
fc+="[2:v]$V,setpts=PTS-STARTPTS[c];"
if [[ "$hook" != "-" ]]; then
  fc+="[0:v]trim=start=$hook:duration=$HOOK,setpts=PTS-STARTPTS,$V[h];[h][s]concat=n=2:v=1:a=0[hs];"
else
  fc+="[s]null[hs];"
fi
off1=$(echo "$hookd + $schd - $XF1" | bc -l)
off2=$(echo "$off1 + $main - $XF2" | bc -l)
fc+="[hs][m]xfade=transition=fade:duration=$XF1:offset=$off1[hm];"
fc+="[hm][c]xfade=transition=fade:duration=$XF2:offset=$off2[v];"

# ---- audio ----
if [[ "$audio" == "norm" ]]; then
  parts=""
  if [[ "$hook" != "-" ]]; then fc+="[0:a]atrim=start=$hook:duration=$HOOK,asetpts=PTS-STARTPTS,$A[ha];"; parts+="[ha]"; fi
  fc+="$(silence "$(echo "$schd - $XF1" | bc -l)")[sa];[0:a]atrim=start=$ss:end=$to,asetpts=PTS-STARTPTS,$A,apad=pad_dur=$FREEZE[ma];$(silence "$(echo "$CARD - $XF2" | bc -l)")[ca];"
  n=$([[ "$hook" != "-" ]] && echo 4 || echo 3)
  fc+="${parts}[sa][ma][ca]concat=n=$n:v=0:a=1,$LOUD[o];"
fi
if [[ "$music" != "-" ]]; then
  fc+="[3:a]atrim=0:$total,asetpts=N/SR/TB,$A,afade=t=in:d=1,afade=t=out:st=$fade_at:d=2,$LOUD"
  if [[ "$audio" == "norm" ]]; then fc+=",volume=0.25[mu];[o][mu]amix=inputs=2:duration=first:normalize=0[a]"; else fc+="[a]"; fi
elif [[ "$audio" == "norm" ]]; then
  fc+="[o]anull[a]"
else
  fc+="$(silence "$total")[a]"
fi

mkdir -p "$(dirname "$out")"
ffmpeg -hide_banner -loglevel error -y "${inputs[@]}" -filter_complex "$fc" -map "[v]" -map "[a]" -t "$total" \
  -c:v libx264 -preset medium -crf 20 -pix_fmt yuv420p -c:a aac -b:a 160k \
  -map_metadata -1 -movflags +faststart "$out"
printf "OK %s (%.1f s: hook %s + schematic %.1f + footage %.1f + card %s)\n" "$out" "$(dur "$out")" "$hookd" "$schd" "$main" "$CARD"
