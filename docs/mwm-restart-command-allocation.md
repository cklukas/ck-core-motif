---
nav_exclude: true
---
# MWM restart-command allocation

Session management rebuilds the window manager restart command with the
private database path.  The pointer-array size and every copied argument are
now checked; partial copies are released before failure is returned.  Valid
restart commands retain their order and `-session` replacement behavior.

`tests/automated/check-mwm-restart-command-allocation.py` protects this
contract.
