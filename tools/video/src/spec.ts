// Wiring description written by Claude for each lesson step: projects/NN-name/steps/stepN.wiring.json
//
// Hole names:
//   breadboard   "a1".."j30"   (row letter + column number)
//   power rails  "+1".."+30", "-1".."-30"  (top rails)
//   Uno pins     "5V", "3V3", "GND", "VIN", "A0".."A5", "D0".."D13"

export type WireColor = "red" | "black" | "yellow" | "orange" | "green" | "blue" | "white" | "purple";
export type LedColor = "red" | "green" | "yellow" | "blue" | "white";

export type Part =
  | { id: string; type: "resistor"; from: string; to: string; label: string }
  | { id: string; type: "led"; anode: string; cathode: string; color?: LedColor; label?: string }
  | { id: string; type: "potentiometer"; legs: [string, string, string]; label?: string }
  | { id: string; type: "photoresistor"; legs: [string, string]; label?: string }
  | { id: string; type: "button"; legs: [string, string]; label?: string }
  | { id: string; type: "generic"; legs: string[]; label: string };

export type Wire = { from: string; to: string; color: WireColor; caption: string };

export type Live = {
  driver: string;      // part id that changes: potentiometer / photoresistor / button
  outputs: string[];   // LED ids whose brightness follows the reading
  readout: string;     // template: {raw} 0..1023, {pwm} 0..255, {volts} 0.00..5.00
};

export type Spec = {
  title: string;
  board: "uno";
  parts: Part[];
  wires: Wire[];
  live?: Live;
};

export type Platform = "arduino" | "esp32" | "stm32" | "rpi";
export type Cover = { badge: string; words: string; platform: Platform; brand?: string };

export type Props = { spec: Spec; cover: Cover };

export const PLATFORM_COLORS: Record<Platform, { bg: string; fg: string }> = {
  arduino: { bg: "#00979d", fg: "#ffffff" },
  esp32: { bg: "#e7352c", fg: "#ffffff" },
  stm32: { bg: "#3cb4e6", fg: "#03234b" },
  rpi: { bg: "#c51a4a", fg: "#ffffff" },
};

export const WIRE_COLORS: Record<WireColor, string> = {
  red: "#e5484d", black: "#1b1f23", yellow: "#f5c518", orange: "#f08a24",
  green: "#30a46c", blue: "#3b82f6", white: "#e8e8e8", purple: "#8e4ec6",
};

export const LED_COLORS: Record<LedColor, string> = {
  red: "#ff3b30", green: "#34c759", yellow: "#ffcc00", blue: "#0a84ff", white: "#f2f2f2",
};

// Timeline (seconds). Every frame is a pure function of t.
export function timings(spec: Spec) {
  const PARTS0 = 1.2, PD = 0.3;
  const W0 = PARTS0 + spec.parts.length * PD + 0.3;
  const WD = 0.45;
  const LIVE = W0 + spec.wires.length * WD + 0.3;
  const END = LIVE + (spec.live ? 3.2 : 1.0);
  return { PARTS0, PD, W0, WD, LIVE, END };
}
