---
nav_exclude: true
---
# Drop-context validation

Destination setup copies the drop callback structure before reading the drag
context's ICC handle.  It now rejects a missing `dragContext` together with a
missing callback structure, so cancellation or malformed external drop events
cannot reach `XtGetValues` or later drop-transfer calls with a null widget.

The focused regression is `tests/automated/check-drop-context-input.py`.
