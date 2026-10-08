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


def etc1(celsius):
    raw = celsius + 40
    return [raw, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF]


# STEP 1: open the bus (the Python version of socket + bind)
#   bus = can.interface.Bus(channel="vcan0", interface="socketcan")
bus = can.interface.Bus(channel="vcan0", interface="socketcan")
print("Engine is connected!")


rpm = 800
coolant = 20
tick = 0

# STEP 2: send ONE engine frame (the same one you send with cansend)
#   id 0x0CF00400, data FF FF FF 40 1F FF FF FF
#   msg = can.Message(arbitration_id=..., data=[...], is_extended_id=True)
#   bus.send(msg)


# STEP 3: send it forever, every 100 ms
#   while True:  ...  time.sleep(0.1)

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
                arbitration_id=0x18FEEE00, data=etc1(coolant), is_extended_id=True
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


# STEP 4: a function   def eec1(rpm):   that returns the 8 data bytes for any rpm
#   raw = int(rpm / 0.125)
#   low byte = raw & 0xFF, high byte = raw >> 8   (bytes 4 and 5, little-endian)
#   then make rpm go up a little every loop


# STEP 5: a function   def et1(celsius):   for the coolant frame (id 0x18FEEE00)
#   byte 1 = celsius + 40, the other 7 bytes = 0xFF
#   send it once per second


# STEP 6: stop cleanly on Ctrl+C
#   try: ... except KeyboardInterrupt: ... finally: bus.shutdown()
