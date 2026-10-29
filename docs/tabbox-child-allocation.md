---
nav_exclude: true
---
# TabBox canvas allocation

`XmTabBox` renders through an internal canvas child. If Xt cannot create that
canvas, initialization now releases the filtered argument list and leaves the
TabBox in a safe, uninitialized state. Resize and tab drawing treat the
missing canvas as an unavailable rendering surface instead of dereferencing a
null child. Normal canvas creation and rendering are unchanged.

The contract is protected by
`tests/automated/check-tabbox-child-allocation.py`.
