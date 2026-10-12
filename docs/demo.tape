# VHS script for the README demo: `vhs docs/demo.tape` -> docs/demo.gif
# Needs: vcan0 up (./scripts/setup_vcan.sh), build/j1939-monitor built, python3-can.

Output docs/demo.gif

Set Shell "bash"
Set FontSize 16
Set Width 1000
Set Height 600
Set Padding 20
Set TypingSpeed 35ms
Set Theme "Builtin Dark"

Hide
Type "cd $(git rev-parse --show-toplevel) && clear" Enter
Sleep 500ms
Show

# Scene 1: the simulated engine warms up and reports two faults over BAM.
Type "python3 tools/ecu_sim.py > /dev/null &   # fake engine ECU on vcan0" Enter
Sleep 800ms
Type "./build/j1939-monitor | grep -v rpm      # hide the 10 Hz rpm lines" Enter
Sleep 12s
Ctrl+C
Sleep 500ms
Hide
Type "kill %1; wait 2>/dev/null; clear" Enter
Sleep 800ms
Show

# Scene 2: a BAM announcement with no data packets is dropped after T1 on a silent bus.
Type "(sleep 1; cansend vcan0 1CECFF00#200A0002FFCAFE00) &   # TP.CM only, then silence" Enter
Type "./build/j1939-monitor" Enter
Sleep 3s
Ctrl+C
Sleep 1s
