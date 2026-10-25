---
nav_exclude: true
---
# XIM reference removal

XIM display data tracks widgets that share input contexts. The old removal
helper decremented the reference count before checking that the widget was in
the list, so a duplicate or stale destroy callback could underflow the count
and corrupt the arrays.

The helper now verifies the reference first and treats an absent widget as an
idempotent no-op. Registered widgets keep the existing swap-and-release cleanup
behavior.

`tests/automated/check-xim-ref-removal.py` guards the lifecycle checks.
