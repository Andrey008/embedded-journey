// The whole picture for a moment t (seconds). mode "cover" = final state + series badge and big words.
import React, { useMemo } from "react";
import { loadFont as loadDisplay } from "@remotion/google-fonts/SpaceGrotesk";
import { loadFont as loadBody } from "@remotion/google-fonts/Inter";
import { loadFont as loadMono } from "@remotion/google-fonts/JetBrainsMono";
import { BOARD, COLS, HEADERS, PINS, RAIL_Y, ROW_Y, UNO, colX, pathD, pathLen, routeAll } from "./geometry";
import { PartView } from "./parts";
import { PLATFORM_COLORS, WIRE_COLORS, timings, type Props } from "./spec";

const DISPLAY = loadDisplay("normal", { weights: ["500", "700"] }).fontFamily;
const BODY = loadBody("normal", { weights: ["400", "500"] }).fontFamily;
const MONO = loadMono("normal", { weights: ["400", "600"] }).fontFamily;

const clamp = (x: number, a = 0, b = 1) => Math.max(a, Math.min(b, x));
const ease = (x: number) => 1 - Math.pow(1 - clamp(x), 3);

// Live reading 0..1 for the driver part
function reading(type: string | undefined, s: number) {
  if (s <= 0) return 0;
  if (type === "button") return Math.floor(s / 0.6) % 2 ? 1 : 0;
  return 0.5 + 0.5 * Math.sin(s * 1.6 - Math.PI / 2);
}

export const Scene: React.FC<Props & { t: number; mode: "video" | "cover" }> = ({ spec, cover, t, mode }) => {
  const T = timings(spec);
  const routes = useMemo(() => routeAll(spec.wires), [spec.wires]);
  const isCover = mode === "cover";
  if (isCover) t = T.END;

  const usedPins = new Set(spec.wires.flatMap((w) => [w.from, w.to]).filter((n) => n in PINS));
  const driver = spec.live ? spec.parts.find((p) => p.id === spec.live!.driver) : undefined;
  const liveS = t - T.LIVE;
  const value = isCover ? 1 : spec.live ? reading(driver?.type, liveS) : 0;
  const raw = Math.round(value * 1023);
  const liveK = isCover ? 1 : clamp(liveS / 0.4);

  const u = isCover ? 1 : ease(t / 0.6);
  const b = isCover ? 1 : ease((t - 0.5) / 0.6);

  // Caption at the bottom
  let caption = "", sub = "";
  if (t < T.PARTS0) caption = "Arduino Uno + breadboard";
  else if (t < T.W0) { caption = "Parts"; sub = spec.parts.map((p) => p.label).filter(Boolean).join(" · "); }
  else if (t < T.LIVE) {
    const i = Math.min(spec.wires.length - 1, Math.floor((t - T.W0) / T.WD));
    caption = spec.wires[i].caption; sub = `wire ${i + 1} / ${spec.wires.length}`;
  } else if (spec.live) {
    caption = driver?.type === "button" ? "Press the button" : driver?.type === "photoresistor" ? "Cover the sensor" : "Turn the knob";
    sub = spec.live.readout
      .replace("{raw}", String(raw).padStart(4))
      .replace("{pwm}", String(raw >> 2).padStart(3))
      .replace("{volts}", ((raw * 5) / 1023).toFixed(2));
  } else caption = spec.title;

  const pc = PLATFORM_COLORS[cover.platform];

  return (
    <svg viewBox="0 0 1080 1920" width={1080} height={1920} style={{ background: "#0c1214" }}>
      <defs>
        <pattern id="grid" width={60} height={60} patternUnits="userSpaceOnUse">
          <path d="M60 0H0V60" fill="none" stroke="#16242a" strokeWidth={2} />
        </pattern>
        <linearGradient id="fade" x1="0" y1="0" x2="0" y2="1">
          <stop offset="0" stopColor="#0c1214" stopOpacity={0} />
          <stop offset="0.35" stopColor="#0c1214" stopOpacity={0.92} />
          <stop offset="1" stopColor="#0c1214" />
        </linearGradient>
      </defs>
      <rect width={1080} height={1920} fill="url(#grid)" />

      {!isCover && (
        <text x={540} y={130} textAnchor="middle" fill="#e8f1f2" fontFamily={DISPLAY} fontSize={58} fontWeight={700}>
          {spec.title}
        </text>
      )}

      {/* Uno */}
      <g opacity={u} transform={`translate(0 ${(1 - u) * -60})`}>
        <rect x={UNO.x} y={UNO.y} width={UNO.w} height={UNO.h} rx={26} fill="#0f6f86" />
        <rect x={140} y={380} width={70} height={110} rx={6} fill="#b8c2c8" />
        <rect x={140} y={590} width={60} height={90} rx={6} fill="#22282c" />
        <rect x={470} y={440} width={300} height={90} rx={6} fill="#1b2226" />
        <text x={620} y={495} textAnchor="middle" fill="#9fb4bb" fontFamily={MONO} fontSize={26}>ATmega328P</text>
        <text x={300} y={560} fill="#e6f4f8" fontFamily={DISPLAY} fontSize={44} fontWeight={700}>UNO</text>
        {HEADERS.map(([x0, y, names]) => (
          <g key={`${x0}-${y}`}>
            <rect x={x0 - 16} y={y - 16} width={names.length * 30 + 2} height={32} rx={4} fill="#151a1d" />
            {names.map((n, i) => (
              <rect key={n} x={x0 + i * 30 - 7} y={y - 7} width={14} height={14} fill={usedPins.has(n) ? "#f5c518" : "#59636a"} />
            ))}
          </g>
        ))}
        {[...usedPins].map((n, k) => {
          const [x, y] = PINS[n];
          const top = y < 500;
          return (
            <text key={n} x={x} y={top ? y + 50 : y - 30 - (k % 2) * 24} textAnchor="middle" fill="#ffffff" fontFamily={MONO} fontSize={22} fontWeight={600}>
              {n.replace("GND2", "GND").replace("GND_TOP", "GND")}
            </text>
          );
        })}
      </g>

      {/* Breadboard */}
      <g opacity={b} transform={`translate(0 ${(1 - b) * 60})`}>
        <rect x={BOARD.x} y={BOARD.y} width={BOARD.w} height={BOARD.h} rx={14} fill="#eceae3" />
        <rect x={BOARD.x} y={1250} width={BOARD.w} height={22} fill="#d9d6cc" />
        <line x1={130} y1={993} x2={950} y2={993} stroke="#e5484d" strokeWidth={3} />
        <line x1={130} y1={1057} x2={950} y2={1057} stroke="#3b82f6" strokeWidth={3} />
        <text x={108} y={1018} fill="#e5484d" fontSize={26} fontWeight={700}>+</text>
        <text x={108} y={1050} fill="#3b82f6" fontSize={30} fontWeight={700}>−</text>
        {Array.from({ length: COLS }, (_, i) => i + 1).map((c) => (
          <g key={c}>
            {c % 6 !== 0 && [RAIL_Y["+"], RAIL_Y["-"]].map((y) => <rect key={y} x={colX(c) - 4.5} y={y - 4.5} width={9} height={9} rx={2} fill="#8d918f" />)}
            {Object.values(ROW_Y).map((y) => <rect key={y} x={colX(c) - 4.5} y={y - 4.5} width={9} height={9} rx={2} fill="#8d918f" />)}
          </g>
        ))}
      </g>

      {/* Parts */}
      {spec.parts.map((p, i) => {
        const k = isCover ? 1 : ease((t - T.PARTS0 - i * T.PD) / 0.4);
        const isOut = spec.live?.outputs.includes(p.id);
        const glow = isOut ? value * liveK : 0;
        return (
          <g key={p.id} opacity={k} transform={`translate(0 ${(1 - k) * -40})`}>
            <PartView part={p} value={p.id === spec.live?.driver ? value : 0} glow={glow} />
          </g>
        );
      })}

      {/* Wires */}
      {spec.wires.map((w, i) => {
        const p = routes[i];
        const d = pathD(p), len = pathLen(p);
        const k = isCover ? 1 : ease((t - T.W0 - i * T.WD) / T.WD);
        const off = len * (1 - k);
        return (
          <g key={i}>
            <path d={d} fill="none" stroke="#000" strokeOpacity={0.35} strokeWidth={16} strokeLinejoin="round" strokeLinecap="round" strokeDasharray={len} strokeDashoffset={off} />
            <path d={d} fill="none" stroke={WIRE_COLORS[w.color]} strokeWidth={10} strokeLinejoin="round" strokeLinecap="round" strokeDasharray={len} strokeDashoffset={off} />
            {spec.live && !isCover && t > T.LIVE && (
              <path d={d} fill="none" stroke="#ffffff" strokeWidth={4} strokeLinecap="round" strokeDasharray="2 26" strokeDashoffset={-liveS * 140} opacity={0.75 * liveK} />
            )}
          </g>
        );
      })}

      {/* Bottom captions (video) or cover overlay */}
      {!isCover ? (
        <g>
          <text x={540} y={1580} textAnchor="middle" fill="#e8f1f2" fontFamily={DISPLAY} fontSize={54} fontWeight={700}>{caption}</text>
          <text x={540} y={1650} textAnchor="middle" fill="#8fb3ba" fontFamily={MONO} fontSize={34} style={{ whiteSpace: "pre" }}>{sub}</text>
          {spec.live && (
            <g opacity={liveK}>
              <rect x={190} y={1700} width={700} height={26} rx={13} fill="#1d2a2f" />
              <rect x={190} y={1700} width={700 * value} height={26} rx={13} fill="#ff5a4f" />
            </g>
          )}
          <text x={540} y={1840} textAnchor="middle" fill="#4d6a71" fontFamily={BODY} fontSize={30}>{cover.brand ?? "Embedded from zero"}</text>
        </g>
      ) : (
        <g>
          <rect x={0} y={1380} width={1080} height={540} fill="url(#fade)" />
          <rect x={64} y={84} width={300} height={100} rx={22} fill={pc.bg} />
          <text x={214} y={154} textAnchor="middle" fill={pc.fg} fontFamily={DISPLAY} fontSize={60} fontWeight={700}>{cover.badge}</text>
          <text x={540} y={1660} textAnchor="middle" fill="#ffffff" fontFamily={DISPLAY} fontSize={150} fontWeight={700} letterSpacing={-2}>{cover.words}</text>
          <text x={540} y={1770} textAnchor="middle" fill="#8fb3ba" fontFamily={BODY} fontSize={46} fontWeight={500}>{cover.brand ?? "Embedded from zero"}</text>
        </g>
      )}
    </svg>
  );
};
