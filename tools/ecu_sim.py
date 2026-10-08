#!/usr/bin/env python3
"""Fake engine ECU: sends J1939 EEC1 (engine speed) and ET1 (coolant) on a CAN bus.

Usage: python3 tools/ecu_sim.py [--channel vcan0]
Stop with Ctrl+C.
"""
import argparse
import time

import can

EEC1_ID = 0x0CF00400  # priority 3, PGN 61444, source address 0x00 (engine)
ET1_ID = 0x18FEEE00   # priority 6, PGN 65262, source address 0x00 (engine)


def eec1(rpm):
    """Engine speed (SPN 190): bytes 4-5, little-endian, 0.125 rpm/bit. None = not available."""
    raw = 0xFFFF if rpm is None else int(rpm / 0.125)
    return bytes([0xFF, 0xFF, 0xFF, raw & 0xFF, raw >> 8, 0xFF, 0xFF, 0xFF])


def et1(celsius):
    """Coolant temperature (SPN 110): byte 1, 1 degC/bit, offset -40. None = not available."""
    raw = 0xFF if celsius is None else int(celsius + 40)
    return bytes([raw, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--channel", default="vcan0")
    args = parser.parse_args()

    bus = can.interface.Bus(channel=args.channel, interface="socketcan")
    print(f"ECU simulator on {args.channel}, Ctrl+C to stop")

    rpm = 800.0
    coolant = 20
    tick = 0
    try:
        while True:
            # Engine speed every 100 ms, ramping 800 -> 2000 rpm.
            # Every 50th frame the sensor reports "not available".
            value = None if tick % 50 == 49 else rpm
            bus.send(can.Message(arbitration_id=EEC1_ID, data=eec1(value), is_extended_id=True))
            rpm = 800.0 if rpm >= 2000 else rpm + 25

            # Coolant temperature every second, warming up to 90 degC.
            if tick % 10 == 0:
                bus.send(can.Message(arbitration_id=ET1_ID, data=et1(coolant), is_extended_id=True))
                coolant = min(coolant + 1, 90)

            tick += 1
            time.sleep(0.1)
    except KeyboardInterrupt:
        pass
    finally:
        bus.shutdown()


if __name__ == "__main__":
    main()
