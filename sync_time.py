# ============================================
# MACROPAD V1 - Sync Orologio + Volume
# ============================================
# pip install pyserial pycaw comtypes
# python sync_time.py
# ============================================

import serial
import serial.tools.list_ports
import time
import sys

def find_arduino():
    ports = serial.tools.list_ports.comports()
    for port in ports:
        desc = port.description.lower()
        mfr = (port.manufacturer or "").lower()
        if any(keyword in desc + mfr for keyword in ['arduino', 'leonardo', 'sparkfun', 'atmega32u4']):
            return port.device
    return None

def get_volume():
    """Legge il volume attuale di Windows (0-100)."""
    try:
        from pycaw.pycaw import AudioUtilities
        d = AudioUtilities.GetSpeakers()
        level = d.EndpointVolume.GetMasterVolumeLevelScalar()
        return int(round(level * 100))
    except Exception:
        return -1

def send_time(ser):
    now = time.localtime()
    time_str = f"T:{now.tm_hour:02d}:{now.tm_min:02d}:{now.tm_sec:02d}\n"
    ser.write(time_str.encode())
    return time_str.strip()

def send_volume(ser):
    vol = get_volume()
    if vol >= 0:
        vol_str = f"V:{vol}\n"
        ser.write(vol_str.encode())
        return vol
    return -1

def main():
    print("MACROPAD V1 - Sync Orologio + Volume")
    print("Premi Ctrl+C per uscire.\n")

    while True:
        port = find_arduino()
        if not port:
            print("Arduino non trovato, riprovo tra 3 secondi...")
            time.sleep(3)
            continue

        print(f"Connessione a {port}...")

        try:
            ser = serial.Serial(port, 9600, timeout=1)
            time.sleep(2)

            ts = send_time(ser)
            vol = send_volume(ser)
            print(f"Connesso! Ora: {ts} | Volume: {vol}%")

            last_sync = time.time()
            last_vol_sync = time.time()

            while True:
                if ser.in_waiting > 0:
                    line = ser.readline().decode('utf-8', errors='ignore').strip()
                    if line:
                        print(f"  Arduino: {line}")

                # Sync ora ogni 30 secondi
                if time.time() - last_sync >= 30:
                    ts = send_time(ser)
                    print(f"Sync ora: {ts}")
                    last_sync = time.time()

                # Sync volume ogni 2 secondi
                if time.time() - last_vol_sync >= 2:
                    vol = send_volume(ser)
                    last_vol_sync = time.time()

                time.sleep(0.1)

        except (serial.SerialException, OSError):
            print("Arduino scollegato, riprovo tra 3 secondi...")
            time.sleep(3)

        except KeyboardInterrupt:
            print("\nChiuso.")
            sys.exit(0)

        finally:
            if 'ser' in locals() and ser.is_open:
                ser.close()

if __name__ == "__main__":
    main()