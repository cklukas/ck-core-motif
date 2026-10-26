---
nav_exclude: true
---

# Image-array size validation

`XmImageArrayFits` now rejects a zero element size before dividing the maximum
representable allocation size.  Normal image-table checks remain unchanged,
while malformed or future direct calls cannot trigger division by zero.

The regression check is
`tests/automated/check-image-array-fit-inputs.py`.
