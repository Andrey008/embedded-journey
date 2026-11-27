#!/usr/bin/env bash
# Render the animated breadboard intro and the cover from a wiring JSON.
#
# Usage: tools/video/render.sh <wiring.json> <out_dir> <badge> <words> <platform>
#   e.g. tools/video/render.sh projects/02-light-theremin/steps/step4.wiring.json /tmp/out "W02 #3" "POT → LED" arduino
# Output: <out_dir>/schematic.mp4 (silent, 1080x1920@30) and <out_dir>/cover.png
set -euo pipefail

here="$(cd "$(dirname "$0")" && pwd)"
spec="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
out="$2"; badge="$3"; words="$4"; platform="${5:-arduino}"
mkdir -p "$out"; out="$(cd "$out" && pwd)"

props="$out/props.json"
node -e '
  const [spec, badge, words, platform, dst] = process.argv.slice(1);
  const s = JSON.parse(require("fs").readFileSync(spec, "utf8"));
  require("fs").writeFileSync(dst, JSON.stringify({ spec: s, cover: { badge, words, platform } }));
' "$spec" "$badge" "$words" "$platform" "$props"

cd "$here"
[[ -d node_modules ]] || npm install --no-audit --no-fund >/dev/null
npx remotion render src/index.ts Schematic "$out/schematic.mp4" --props="$props" --log=error
npx remotion still  src/index.ts Cover     "$out/cover.png"     --props="$props" --log=error
echo "OK $out/schematic.mp4 $out/cover.png"
