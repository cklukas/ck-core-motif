---
nav_exclude: true
---

# Clipboard event-message Atom comparisons

`ClipboardEventHandler` now rejects request/delete Atom IDs that cannot fit
the signed client-message payload representation and compares representable
IDs explicitly as `long` values.

Regression: `tests/automated/check-clipboard-event-message-atoms.py`
