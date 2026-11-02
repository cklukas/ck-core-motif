---
nav_exclude: true
---
# MWM restart-command allocation

Session management rebuilds the window manager restart command with the
private database path. The pointer-array element count is checked against
both `size_t` and Xt's `Cardinal` allocation width, and every copied argument
is checked; partial copies are released before failure is returned. Valid
restart commands retain their order and `-session` replacement behavior.

Counting also rejects a command vector whose loop or optional `-session`
replacement would overflow its legacy `int` count.

`tests/automated/check-mwm-restart-command-allocation.py` protects this
contract.
