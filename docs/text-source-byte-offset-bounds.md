---
nav_exclude: true
---
# Text source byte-offset bounds

The legacy gap-buffer source converts text positions into byte offsets for
multibyte and wide-character storage.  That conversion is now centralized and
checks the position, character width, source capacity, and `size_t` product
before pointer arithmetic.  Reads, gap movement, and character lookup all use
the checked helper; internal gap distances are `size_t`, while the public
source callbacks and Motif ABI remain unchanged.

`tests/automated/check-text-source-byte-offset.py` is the focused regression
for the checked conversion and widened gap distances.
