---
nav_exclude: true
---
# External selection-owner lifecycle

Motif transfer helpers can replace a selection owned by an unrelated X11
window.  The replacement must become the Motif widget's owner, and destroying
that widget must release the selection instead of leaving a stale owner behind.

The focused regression creates a foreign X11 owner, claims the same named
selection through `XmeNamedSource`, verifies the replacement, destroys the
Motif owner, and verifies that the selection is cleared:

```sh
python3 tests/automated/run-external-selection-regressions.py \
  /absolute/prefix /absolute/results
```
