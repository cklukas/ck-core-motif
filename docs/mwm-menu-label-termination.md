---
nav_exclude: true
---
# Mwm menu-label termination

Client-command menu labels are parsed from quoted resource text.  The legacy
parser copied the bytes with `strncpy` without terminating them, then called
`strlen` on the unterminated allocation and wrote the terminator one byte past
the intended label.  An empty quoted label also left the multibyte scan length
uninitialized.

The parser now initializes the scan state and copies the exact label length
with an explicit terminator.  Empty, ASCII, and multibyte labels all have a
deterministic owned string.

`tests/automated/check-mwm-menu-label-termination.py` protects this contract.
