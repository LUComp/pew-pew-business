# Pew Pew Business

A pan/tilt tracking turret that uses computer vision to find a target and aim at it automatically.

The mechanical design is based on the Thingiverse model
[thing:1154361](https://www.thingiverse.com/thing:1154361). Please check the
original page for its license and credit the author when sharing derived parts.

## How it works

```
 Camera ──► Computer vision (Python, host PC) ──► USB serial ──► Arduino ──► 2x stepper motors (pan / tilt)
```

1. The camera streams frames to the host computer.
2. The vision code detects the target and computes its offset from the frame centre.
3. The offset is converted to pan/tilt step counts and sent over USB serial.
4. The Arduino drives the steppers to re-centre the target.

## Hardware

- Arduino (serial at 115200 baud)
- 2x 28BYJ-48 stepper motors with ULN2003 driver boards (pan and tilt)
- USB camera / webcam
- 3D-printed turret parts (see `hardware/`)

### Pin mapping

| Axis | Arduino pins (IN1, IN3, IN2, IN4) |
|------|-----------------------------------|
| Pan  | 8, 10, 9, 11 |
| Tilt | 2, 4, 3, 5 |

## Repository layout

```
.
├── README.md
├── firmware/                  # Arduino sketches (C++)
│   └── basic_movements/
│       ├── move1000/          # Simple move test
│       └── control_by_keyboard/  # Jog the turret from the Serial Monitor
├── vision/                    # Computer-vision code (Python): detection, tracking, serial output
├── hardware/
│   ├── stl/                   # 3D-printable parts
│   └── wiring/                # Wiring diagrams and pin notes
└── docs/                      # Notes, photos, design decisions
```

## Getting started

### 1. Firmware

1. Install the Arduino IDE and the **AccelStepper** library (Library Manager).
2. Open `firmware/basic_movements/control_by_keyboard/control_by_keyboard.ino`, upload it.
3. Open the Serial Monitor at **115200 baud** and test:

| Key | Action |
|-----|--------|
| `w` / `s` | Tilt up / down |
| `a` / `d` | Pan left / right |
| `space` | Stop |
| `h` | Return to home (startup position) |

Repeat a letter for larger moves (`ddd` = three jogs right). If an axis moves the wrong way,
flip `PAN_DIR` / `TILT_DIR` in the sketch.

### 2. Serial protocol (for the vision code)

Send a newline-terminated relative move:

```
P<steps>,T<steps>      e.g.  P-40,T12
```

The Arduino replies `D` when both motors have finished moving. 4096 half-steps = one full turn.

### 3. Vision

Not implemented yet. Code will live in `vision/`.

## Roadmap

- [x] Basic pan/tilt movement and keyboard control
- [x] Assemble and wire the printed turret
- [ ] Remote control
- [ ] Camera capture and target detection
- [ ] Convert pixel offset to step commands
- [ ] Closed-loop tracking
- [ ] Calibrate limits and speed

## Credits

Base turret design: [Thingiverse thing:1154361](https://www.thingiverse.com/thing:1154361).
