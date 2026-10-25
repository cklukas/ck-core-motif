---
nav_exclude: true
---

# WSM property wire width

WM-window properties are serialized with `PackProperty` as 32-bit protocol
fields.  Request and reply size calculations now use `sizeof(CARD32)` rather
than host `Window`/`WindowProperty` widths, preventing oversized messages and
uninitialized padding from being sent on 64-bit platforms.
