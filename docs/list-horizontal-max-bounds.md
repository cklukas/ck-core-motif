---
nav_exclude: true
---

# List horizontal maximum bounds

List computes the horizontal content maximum in a wide intermediate type and
clamps it to the navigator’s signed range. This prevents `BaseX * 2` or the
content width from overflowing before scrollbar range normalization.
