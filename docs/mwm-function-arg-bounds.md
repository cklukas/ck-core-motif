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
unchanged. The formatter result remains signed until negative errors are
rejected, then is checked against Xt's byte-count limit before allocation.
Null command-tree and cascade-name inputs fail safely instead of reaching the
formatter.

`tests/automated/check-mwm-function-arg-bounds.py` protects this contract.
