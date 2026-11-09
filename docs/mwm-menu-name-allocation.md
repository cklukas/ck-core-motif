---
nav_exclude: true
---
# MWM quoted menu-name allocation

MWM resource parsing accepts a quoted label for client-command menu entries.
The parser requires the matching closing quote before it measures or stores the
label, then checks the measured name length and allocation result before
copying the label or writing its terminator. An unterminated, unrepresentable,
or unallocatable label is ignored safely; valid labels and command matching are
unchanged.

`tests/automated/check-mwm-menu-name-allocation.py` protects this contract.
`tests/automated/run-mwm-menu-name-termination.py` compiles the real parser
into a small harness covering complete and unterminated labels.
