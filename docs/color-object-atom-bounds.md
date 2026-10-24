# Color object atom-name bounds

Color objects create one per-screen atom name from the fixed Motif resource
prefix and the screen index. The name slots are 24 bytes; formatting now uses
`snprintf` with that capacity, preserving normal atom names while preventing an
unexpectedly large index from writing beyond a slot.

`tests/automated/check-color-object-atom-bounds.py` guards the formatting path.
