# 🎛️ MacroPad DIY V1

A custom 9-key macropad with rotary encoder and OLED display, powered by Arduino Pro Micro.

## ✨ Features

- **9 mechanical keys** with customizable macros (shortcuts, git commands, key combinations)
- **EC11 rotary encoder** for volume control with mute/unmute toggle
- **1.3" OLED display** with fixed header (name + clock), animated volume bar, and visual feedback for each key press
- **Clock sync** from PC via USB Serial
- **Volume sync** from PC for an always-accurate volume bar
- **Anti-flood and noise protection** to prevent Arduino lockups

## 🔧 Hardware Components

| Component | Specification | Notes |
|---|---|---|
| Arduino Pro Micro | ATmega32U4 5V/16MHz | Native HID support (keyboard/mouse over USB) |
| OLED Display | 1.3" SH1106 I2C | I2C address: 0x3C (verify with I2C scanner) |
| Rotary Encoder | EC11E12 (5 pins) | 4 pulses per detent, with built-in push button |
| Mechanical Switches | Cherry MX (x9) | Any variant (Red, Blue, Brown, etc.) |
| Keycaps | Cherry MX compatible (x9) | Any profile (OEM, DSA, SA, etc.) |
| Perfboard / PCB | Optional | Recommended for encoder soldering |
| DuPont Wires | Male-to-Male | For breadboard/PCB connections |

## 🔌 Wiring Diagram

### Arduino Pro Micro Pin Table

| Component | Component Pin | Arduino Pin | Notes |
|---|---|---|---|
| **Encoder CLK** | A (left, 3-pin side) | **0 (TX)** | Hardware interrupt ✓ |
| **Encoder DT** | B (right, 3-pin side) | **1 (RX)** | Hardware interrupt ✓ |
| **Encoder GND** | C (center, 3-pin side) | **GND** | |
| **Encoder SW** | Either pin (2-pin side) | **14** | Mute button |
| **Encoder SW GND** | Other pin (2-pin side) | **GND** | Interchangeable |
| **Display SDA** | SDA | **2 (SDA)** | I2C - dedicated pin |
| **Display SCL** | SCL | **3 (SCL)** | I2C - dedicated pin |
| **Display VCC** | VCC | **VCC** | 5V or 3.3V depending on display |
| **Display GND** | GND | **GND** | |
| **Key 1** | One pin | **4** | Other pin to GND |
| **Key 2** | One pin | **5** | Other pin to GND |
| **Key 3** | One pin | **6** | Other pin to GND |
| **Key 4** | One pin | **7** | Other pin to GND |
| **Key 5** | One pin | **8** | Other pin to GND |
| **Key 6** | One pin | **9** | Other pin to GND |
| **Key 7** | One pin | **15** | Other pin to GND |
| **Key 8** | One pin | **A0** | Other pin to GND |
| **Key 9** | One pin | **A1** | Other pin to GND |

> ⚠️ **IMPORTANT**: Pins 2 and 3 are reserved exclusively for the I2C display. Do NOT connect any other components to these pins.

### Wiring Notes

- All Cherry MX switches have 2 pins: one goes to the designated Arduino pin, the other to GND
- Multiple components can share the same GND pin on the Arduino
- The EC11 encoder has 3 pins on one side (rotation) and 2 pins on the other (button). The center pin on the 3-pin side is always GND
- If the volume direction is reversed, swap pins 0 and 1 in the code: `Encoder myEnc(1, 0);`

## ⌨️ Macro Map

| Key | Action | Shortcut | Use |
|---|---|---|---|
| 1 | Screenshot | Win+Shift+S | Partial screen capture |
| 2 | Task Manager | Ctrl+Shift+Esc | Opens Task Manager |
| 3 | Open CMD | Win+R → cmd → Enter | Opens Command Prompt |
| 4 | Git Status | `git status` + Enter | Repository status |
| 5 | Git Pull | `git pull` + Enter | Pull latest changes |
| 6 | Git Commit & Push | `git add . && git commit -m "" && git push` | Places cursor between quotes for commit message |
| 7 | Format Document | Shift+Alt+F | Auto-format (VS Code) |
| 8 | Search in Files | Ctrl+Shift+F | Global search (VS Code) |
| 9 | Toggle Comment | Ctrl+/ | Comment/uncomment line (VS Code) |
| Encoder | Volume | Rotation | Volume up/down |
| Encoder Press | Mute | Press | Toggle mute/unmute |

## 📦 Installation

### 1. Arduino IDE Setup

1. Open Arduino IDE
2. Go to **Tools → Board**
3. Select **Arduino Leonardo**
   > ⚠️ The Arduino IDE does not natively recognize the Pro Micro. If "Arduino Pro Micro" is not listed in the board menu, select **Arduino Leonardo** instead — it is 100% compatible since both boards use the same ATmega32U4 chip.
4. Select the correct COM port under **Tools → Port**

### 2. Arduino Libraries

Install the following libraries via **Tools → Manage Libraries**:

- `Adafruit GFX Library`
- `Adafruit SH110X`
- `HID-Project`
- `Encoder`

### 3. Upload the Sketch

1. Open `macropadDIY.ino` in the IDE
2. Click **Upload**

> **Upload troubleshooting**: The Pro Micro may require a double reset to enter bootloader mode. If the upload fails with "Access denied" or similar errors:
> 1. Click **Upload** in the IDE
> 2. When the console shows "Sketch uses XXX bytes..."
> 3. Quickly connect **RST to GND twice** (touch, release, touch, release in under 1 second)
> 4. The IDE detects the bootloader and completes the upload

### 4. PC Sync Script (Python)

The Python script sends the current time and real system volume to the macropad.

**Requirements:**
```bash
pip install pyserial pycaw comtypes
```

**Manual launch:**
```bash
pythonw "path/to/sync_time.py"
```

**Included .bat files:**

| File | Function |
|---|---|
| `start_sync.bat` | Starts the script in background (no window) |
| `stop_sync.bat` | Stops the script |
| `macropad_sync.bat` | For automatic startup with Windows |

**Auto-start on boot:**
1. Press `Win + R`
2. Type `shell:startup` and press Enter
3. Copy `macropad_sync.bat` into the folder that opens

## 🖨️ 3D Printed Case

The case is designed to house all components. STL files for 3D printing are located in the `/3d-models/` folder.

Recommended settings: PLA or PETG, 0.2mm layer height, 20% infill.
> 💡 **Assembly tip**: You may need to slightly file down the corners of the key grid to fit it into the bottom case. Sand gradually and test fit frequently.

## 📁 Project Structure

```
macropadDIY/
├── macropadDIY.ino        # Main Arduino sketch
├── sync_time.py           # Python script for time & volume sync
├── start_sync.bat         # Start sync in background
├── stop_sync.bat          # Stop sync
├── macropad_sync.bat      # For Windows auto-startup
├── 3d-models/             # STL files for the case
│   ├── case-top.stl
│   └── case-bottom.stl
└── README.md              # This documentation
```

## 🐛 Troubleshooting

**Display not turning on:**
- Check SDA (pin 2) and SCL (pin 3) connections
- Try the alternate I2C address `0x3D` in the code
- Reset the Arduino after connecting the display

**Volume goes crazy or only moves in one direction:**
- Check encoder solder joints (especially the 3 rotation pins)
- Add 100nF ceramic capacitors between pin A/GND and pin B/GND
- Verify the encoder pins are on 0 (TX) and 1 (RX)

**"Access denied" error during upload:**
- Close the Serial Monitor and any program using the COM port
- Stop `sync_time.py` with `stop_sync.bat`
- Perform a double reset (RST → GND twice)

**Clock shows --:--:**
- The `sync_time.py` script is not running
- Start it with `start_sync.bat`
- Make sure no other program is using the COM port

**Keys type wrong characters:**
- Check the Windows keyboard layout (the sketch is optimized for US International)
- Special characters (`&`, `"`, `/`) may vary depending on the layout

## 📄 License

MIT License
