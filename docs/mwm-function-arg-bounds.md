---
nav_exclude: true
---
# Mwm menu function-argument bounds

Menu insertion derives cascade names and invoke-command arguments from
user-defined command trees and X11 identifiers.  The legacy insertion path
placed them in a fixed 256-byte buffer, so long cascade names could overflow
stack storage.

Both argument forms now use measured dynamic allocation and are released after
matching completes.  Menu matching and command dispatch content are
unchanged.

`tests/automated/check-mwm-function-arg-bounds.py` protects this contract.
