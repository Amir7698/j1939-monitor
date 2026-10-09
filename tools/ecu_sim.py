#!/usr/bin/env python3
"""Fake engine ECU: sends J1939 engine speed and coolant temperature on vcan0.

Usage: python3 tools/ecu_sim.py
Stop with Ctrl+C.
"""

import time

import can


def eec1(rpm):
    raw = int(rpm / 0.125)
    low = raw & 0xFF
    high = raw >> 8
    return [0xFF, 0xFF, 0xFF, low, high, 0xFF, 0xFF, 0xFF]


def et1(celsius):
    raw = celsius + 40
    return [raw, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]


def dm1(hot):
    if hot:
        return [0x04, 0xFF, 110, 0x00, 0x00, 0x01, 0xFF, 0xFF]
    return [0x00, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF]


bus = can.interface.Bus(channel="vcan0", interface="socketcan")
print("Engine is connected!")


rpm = 800
coolant = 80
tick = 0


try:
    while True:
        msg = can.Message(
            arbitration_id=0x0CF00400,
            data=eec1(rpm),
            is_extended_id=True,
        )
        bus.send(msg)
        if tick % 10 == 0:
            msg = can.Message(
                arbitration_id=0x18FEEE00, data=et1(coolant), is_extended_id=True
            )
            bus.send(msg)
            msg = can.Message(
                arbitration_id=0x18FECA00,
                data=dm1(coolant >= 90),
                is_extended_id=True,
            )
            bus.send(msg)
            if coolant < 90:
                coolant = coolant + 1
        print("rpm", rpm)
        rpm = rpm + 25
        if rpm > 2000:
            rpm = 800
        tick += 1
        time.sleep(0.1)
except KeyboardInterrupt:
    print("engine stopped")

finally:
    bus.shutdown()
