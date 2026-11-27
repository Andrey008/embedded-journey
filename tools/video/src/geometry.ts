// Coordinates of the breadboard and the Uno on a 1080x1920 canvas, plus automatic wire routing.
import type { Wire } from "./spec";

export type Pt = [number, number];

export const STEP = 27.5;                                 // breadboard pitch (2.54 mm) in pixels
export const COLS = 30;
export const colX = (c: number) => 125 + (c - 1) * STEP;
export const ROW_Y: Record<string, number> = {
  a: 1100, b: 1130, c: 1160, d: 1190, e: 1220,           // top half
  f: 1300, g: 1330, h: 1360, i: 1390, j: 1420,           // bottom half (after the trench)
};
export const RAIL_Y = { "+": 1010, "-": 1040 };
export const BOARD = { x: 100, y: 975, w: 880, h: 480 };
export const UNO = { x: 140, y: 260, w: 800, h: 500 };

// Uno headers: [first pin x, y, pin names left to right]
export const HEADERS: [number, number, string[]][] = [
  [400, 292, ["SCL", "SDA", "AREF", "GND_TOP", "D13", "D12", "D11", "D10", "D9", "D8"]],
  [720, 292, ["D7", "D6", "D5", "D4", "D3", "D2", "D1", "D0"]],
  [300, 728, ["NC", "IOREF", "RESET", "3V3", "5V", "GND", "GND2", "VIN"]],
  [600, 728, ["A0", "A1", "A2", "A3", "A4", "A5"]],
];
export const PINS: Record<string, Pt> = {};
for (const [x0, y, names] of HEADERS) names.forEach((n, i) => (PINS[n] = [x0 + i * 30, y]));

export const isUnoPin = (n: string) => n in PINS;
export const isTopPin = (n: string) => isUnoPin(n) && PINS[n][1] < 500;

export function pt(n: string): Pt {
  if (PINS[n]) return PINS[n];
  const m = /^([a-j+\-])(\d{1,2})$/.exec(n);
  if (!m) throw new Error(`Unknown hole or pin "${n}"`);
  const c = Number(m[2]);
  if (c < 1 || c > COLS) throw new Error(`Column out of range in "${n}" (1..${COLS})`);
  const y = m[1] === "+" || m[1] === "-" ? RAIL_Y[m[1]] : ROW_Y[m[1]];
  return [colX(c), y];
}

// ---------- Routing ----------
// Wires from the Uno: down (or up and around the side) to a horizontal "lane" between
// the Uno and the breadboard, across to the target column, then down into the hole.
// Lanes are assigned by trying every order and keeping the one with the fewest crossings.

const LANE0 = 795, LANE_STEP = 22;
type Seg = [number, number, number, number];

function unoPath(w: Wire, lane: number, topIdx: number): Pt[] {
  const [px, py] = pt(w.from);
  const [tx, ty] = pt(w.to);
  const ly = LANE0 + lane * LANE_STEP;
  if (!isTopPin(w.from)) return [[px, py], [px, ly], [tx, ly], [tx, ty]];
  const up = 225 - topIdx * 18;
  const side = tx >= 540 ? 1005 + topIdx * 16 : 75 - topIdx * 16;
  return [[px, py], [px, up], [side, up], [side, ly], [tx, ly], [tx, ty]];
}

function segs(p: Pt[]): Seg[] {
  const s: Seg[] = [];
  for (let i = 1; i < p.length; i++) s.push([p[i - 1][0], p[i - 1][1], p[i][0], p[i][1]]);
  return s;
}

function crosses(a: Seg, b: Seg): boolean {
  const aH = a[1] === a[3], bH = b[1] === b[3];
  if (aH === bH) return false;
  const [h, v] = aH ? [a, b] : [b, a];
  const x = v[0], y = h[1];
  const inH = x > Math.min(h[0], h[2]) && x < Math.max(h[0], h[2]);
  const inV = y > Math.min(v[1], v[3]) && y < Math.max(v[1], v[3]);
  return inH && inV;
}

function countCrossings(paths: Pt[][]): number {
  let n = 0;
  const all = paths.map(segs);
  for (let i = 0; i < all.length; i++)
    for (let j = i + 1; j < all.length; j++)
      for (const a of all[i]) for (const b of all[j]) if (crosses(a, b)) n++;
  return n;
}

function* permutations(n: number): Generator<number[]> {
  const a = [...Array(n).keys()];
  const c = Array(n).fill(0);
  yield a.slice();
  let i = 0;
  while (i < n) {
    if (c[i] < i) {
      const k = i % 2 ? c[i] : 0;
      [a[i], a[k]] = [a[k], a[i]];
      yield a.slice();
      c[i]++; i = 0;
    } else { c[i] = 0; i++; }
  }
}

export function routeAll(wires: Wire[]): Pt[][] {
  const out: Pt[][] = wires.map((w) => [pt(w.from), pt(w.to)]);   // jumpers: straight
  const uno = wires.map((w, i) => (isUnoPin(w.from) ? i : -1)).filter((i) => i >= 0);
  const topIdx: Record<number, number> = {};
  uno.filter((i) => isTopPin(wires[i].from)).forEach((i, k) => (topIdx[i] = k));

  let best: Pt[][] = [], bestScore = Infinity;
  const tryOrder = (order: number[]) => {
    const paths = uno.map((wi, k) => unoPath(wires[wi], order[k], topIdx[wi] ?? 0));
    const score = countCrossings(paths);
    if (score < bestScore) { bestScore = score; best = paths; }
  };
  if (uno.length <= 7) for (const p of permutations(uno.length)) { tryOrder(p); if (bestScore === 0) break; }
  else tryOrder(uno.map((_, k) => k));
  uno.forEach((wi, k) => (out[wi] = best[k]));
  return out;
}

export const pathD = (p: Pt[]) => "M" + p.map((q) => q.join(" ")).join(" L");
export const pathLen = (p: Pt[]) =>
  p.slice(1).reduce((s, q, i) => s + Math.hypot(q[0] - p[i][0], q[1] - p[i][1]), 0);
