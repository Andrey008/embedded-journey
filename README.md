# Embedded Journey

Learning embedded development from scratch: from Arduino to STM32, FreeRTOS, radio links and embedded Linux.
A 16-week hands-on plan (November 2026 – March 2027): minimum theory, maximum practice.

## Projects

| # | Project | Platform | What's inside | Video |
|---|---------|----------|---------------|-------|
| 01 | [Traffic light with pedestrian button](projects/01-traffic-light/) | Arduino Uno | GPIO, `millis()` state machine, debouncing, 7-segment display | [▶️](https://www.youtube.com/watch?v=e7iWiRZsFYM) |
| 02 | [Light theremin](projects/02-light-theremin/) | Arduino Uno | Oscilloscope, PWM, ADC, voltage divider, photoresistor, `tone()`, RGB mixing, calibration | [▶️](https://youtube.com/shorts/sSS71Y4ssPM) |

<!-- Add a row after each mini-project -->

## Repository layout

```
.
├── README.md          ← this file, project showcase
├── HARDWARE.md        ← hardware inventory: boards, sensors, voltages
├── BUGS.md            ← bug journal: what broke and how I fixed it
├── hardware/photos/   ← photos of boards and modules
├── docs/              ← notes, datasheets, calculations
└── projects/
    ├── _template/     ← template for a new project
    └── NN-name/       ← one project = one folder (01-traffic-light, 02-light-theremin…)
        ├── README.md  ← what it does, wiring, what I learned
        ├── src/       ← code (PlatformIO / CubeIDE project)
        └── media/     ← photos, schematics, GIFs, oscilloscope screenshots
```

## Starting a new project

```bash
cp -r projects/_template projects/02-light-theremin
```

## Skills

Arduino · ESP32 · STM32 (HAL, CubeMX) · FreeRTOS · GPIO/PWM/ADC · I2C/SPI/UART · DMA · PID · nRF24/LoRa · Raspberry Pi / Linux · oscilloscope, logic analyzer, soldering
