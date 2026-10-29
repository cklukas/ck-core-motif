---
nav_exclude: true
---
# ClipWindow translation inputs

The private ClipWindow translation builder now rejects a missing display,
missing key-binding array, or non-positive key count before indexing the
array or asking X11 to resolve keysyms.  This also prevents a negative signed
count from being compared against the unsigned traversal index and becoming a
large out-of-bounds walk.

Invalid input returns the existing reusable empty translation buffer, so the
normal widget initialization contract remains fail-closed and ABI-compatible.
The focused source regression is
`tests/automated/check-clip-window-translation-inputs.py`.
