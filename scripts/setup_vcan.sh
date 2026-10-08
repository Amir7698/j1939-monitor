#!/usr/bin/env bash
# Create the virtual CAN interface used by j1939-monitor and the ECU simulator.
# Usage: ./scripts/setup_vcan.sh [interface]   (default: vcan0)
set -euo pipefail

IFACE="${1:-vcan0}"

sudo modprobe -a can can-raw can-dev

if ! sudo modprobe vcan 2>/dev/null; then
    # The WSL2 kernel ships without vcan; fall back to a locally built module.
    sudo insmod "$HOME/wsl-kernel/drivers/net/can/vcan.ko" 2>/dev/null || true
fi

if ! ip link show "$IFACE" >/dev/null 2>&1; then
    sudo ip link add dev "$IFACE" type vcan
fi
sudo ip link set up "$IFACE"

ip -brief link show "$IFACE"
