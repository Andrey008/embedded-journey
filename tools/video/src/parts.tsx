// Drawings of breadboard parts. Each part is placed by the holes its legs go into.
import React from "react";
import { pt, type Pt } from "./geometry";
import { LED_COLORS, type Part } from "./spec";

const LEG = "#9aa0a3";
const MONO = "JetBrains Mono, monospace";

const Label: React.FC<{ x: number; y: number; text?: string }> = ({ x, y, text }) =>
  text ? (
    <text x={x} y={y} textAnchor="middle" fill="#1b1f23" fontFamily={MONO} fontSize={24} fontWeight={600}>
      {text}
    </text>
  ) : null;

// Resistor color bands from a label like "220Ω", "10kΩ", "4.7k"
const BAND = ["#1b1f23", "#7b4a26", "#d6392b", "#f08a24", "#f5c518", "#30a46c", "#3b82f6", "#8e4ec6", "#8d8d8d", "#f2f2f2"];
function bands(label: string): string[] {
  const m = /([\d.]+)\s*([kKmM]?)/.exec(label);
  if (!m) return [];
  let ohms = parseFloat(m[1]) * (m[2].toLowerCase() === "k" ? 1e3 : m[2].toLowerCase() === "m" ? 1e6 : 1);
  if (!(ohms >= 10)) return [];
  let exp = 0;
  while (ohms >= 100) { ohms /= 10; exp++; }
  const d = Math.round(ohms);
  return [BAND[Math.floor(d / 10)], BAND[d % 10], BAND[exp], "#c9a227"];
}

export const PartView: React.FC<{ part: Part; value: number /* 0..1 live reading */; glow: number }> = ({ part, value, glow }) => {
  switch (part.type) {
    case "resistor": {
      const [a, b] = [pt(part.from), pt(part.to)];
      const mx = (a[0] + b[0]) / 2, my = (a[1] + b[1]) / 2;
      const ang = (Math.atan2(b[1] - a[1], b[0] - a[0]) * 180) / Math.PI;
      const bs = bands(part.label);
      return (
        <g>
          <line x1={a[0]} y1={a[1]} x2={b[0]} y2={b[1]} stroke={LEG} strokeWidth={6} />
          <g transform={`translate(${mx} ${my}) rotate(${ang})`}>
            <rect x={-45} y={-16} width={90} height={32} rx={14} fill="#d8b98a" />
            {bs.map((c, i) => <rect key={i} x={-27 + i * 16 + (i === 3 ? 6 : 0)} y={-16} width={7} height={32} fill={c} />)}
          </g>
          <Label x={mx} y={my - 26} text={part.label} />
        </g>
      );
    }
    case "led": {
      const [a, c] = [pt(part.anode), pt(part.cathode)];
      const mx = (a[0] + c[0]) / 2, y = Math.max(a[1], c[1]);
      const col = LED_COLORS[part.color ?? "red"];
      return (
        <g>
          <line x1={a[0]} y1={a[1]} x2={a[0]} y2={y + 42} stroke={LEG} strokeWidth={6} />
          <line x1={c[0]} y1={c[1]} x2={c[0]} y2={y + 42} stroke={LEG} strokeWidth={6} />
          <circle cx={mx} cy={y + 62} r={130} fill={`url(#glow-${part.id})`} opacity={glow} />
          <defs>
            <radialGradient id={`glow-${part.id}`}>
              <stop offset="0" stopColor={col} stopOpacity={0.95} />
              <stop offset="0.45" stopColor={col} stopOpacity={0.35} />
              <stop offset="1" stopColor={col} stopOpacity={0} />
            </radialGradient>
          </defs>
          <path d={`M${mx - 28} ${y + 42} h56 v18 a28 28 0 0 1 -56 0 z`} fill={col} opacity={0.75 + 0.25 * glow} />
          <Label x={mx + 78} y={y + 72} text={part.label} />
        </g>
      );
    }
    case "potentiometer": {
      const ps = part.legs.map(pt);
      const mx = ps[1][0], y = Math.max(...ps.map((p) => p[1]));
      return (
        <g>
          {ps.map((p, i) => <line key={i} x1={p[0]} y1={p[1]} x2={p[0]} y2={y + 42} stroke={LEG} strokeWidth={7} />)}
          <rect x={mx - 90} y={y + 35} width={180} height={110} rx={12} fill="#2f6fb3" />
          <g transform={`translate(${mx} ${y + 90}) rotate(${-135 + 270 * value})`}>
            <circle r={44} fill="#222a30" stroke="#11161a" strokeWidth={4} />
            <line x1={0} y1={0} x2={0} y2={-36} stroke="#f5f5f5" strokeWidth={7} strokeLinecap="round" />
          </g>
          <Label x={mx} y={y + 185} text={part.label} />
        </g>
      );
    }
    case "photoresistor": {
      const ps = part.legs.map(pt);
      const mx = (ps[0][0] + ps[1][0]) / 2, y = Math.max(ps[0][1], ps[1][1]);
      return (
        <g>
          {ps.map((p, i) => <line key={i} x1={p[0]} y1={p[1]} x2={p[0]} y2={y + 40} stroke={LEG} strokeWidth={6} />)}
          <circle cx={mx} cy={y + 62} r={30} fill="#e9d8b4" stroke="#b79b62" strokeWidth={3} />
          <path d={`M${mx - 18} ${y + 50} h30 v8 h-30 v8 h30 v8 h-30`} fill="none" stroke="#b0412c" strokeWidth={4} />
          <circle cx={mx} cy={y + 62} r={80} fill="#fff6c8" opacity={0.55 * value} />
          <Label x={mx} y={y + 125} text={part.label} />
        </g>
      );
    }
    case "button": {
      const ps = part.legs.map(pt);
      const mx = (ps[0][0] + ps[1][0]) / 2, my = (ps[0][1] + ps[1][1]) / 2;
      const pressed = value > 0.5;
      return (
        <g>
          <rect x={mx - 48} y={my - 48} width={96} height={96} rx={8} fill="#2a2f34" />
          <circle cx={mx} cy={my} r={pressed ? 24 : 28} fill={pressed ? "#3a4148" : "#4a525a"} />
          <Label x={mx} y={my + 80} text={part.label} />
        </g>
      );
    }
    case "generic": {
      const ps: Pt[] = part.legs.map(pt);
      const xs = ps.map((p) => p[0]), ys = ps.map((p) => p[1]);
      const x0 = Math.min(...xs) - 30, x1 = Math.max(...xs) + 30, y0 = Math.min(...ys) - 30, y1 = Math.max(...ys) + 30;
      return (
        <g>
          <rect x={x0} y={y0} width={x1 - x0} height={y1 - y0} rx={8} fill="#2a2f34" />
          <text x={(x0 + x1) / 2} y={(y0 + y1) / 2 + 8} textAnchor="middle" fill="#e8f1f2" fontFamily={MONO} fontSize={22}>
            {part.label}
          </text>
        </g>
      );
    }
  }
};
