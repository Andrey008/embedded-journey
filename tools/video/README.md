# Video intro renderer

Animated breadboard schematic + cover for Shorts, rendered with [Remotion](https://www.remotion.dev/) from a wiring JSON.

- Format of the JSON: [src/spec.ts](src/spec.ts). Example: [projects/02-light-theremin/steps/step4.wiring.json](../../projects/02-light-theremin/steps/step4.wiring.json)
- Render: `tools/video/render.sh <wiring.json> <out_dir> "W02 #3" "POT → LED" arduino` → `schematic.mp4` + `cover.png`
- Preview while editing: `cd tools/video && npm run studio`
- Adding a part: draw it in [src/parts.tsx](src/parts.tsx) and add its type to `Part` in `spec.ts`.
- Wires from the Uno are routed automatically (lane order with the fewest crossings, see [src/geometry.ts](src/geometry.ts)).

The final Short is put together by [../assemble.sh](../assemble.sh): hook → schematic → footage → cover, music over everything.
