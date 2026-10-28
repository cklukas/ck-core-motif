---
nav_exclude: true
---

# UIL gadget-count bounds

Control-list records store their gadget count in an unsigned 16-bit field.
The semantic validator now rejects a negative or over-wide accumulated count
before narrowing it, so an unusually large control list cannot wrap and leave
the resource record describing the wrong number of gadgets.

Regression: `tests/automated/check-uil-gadget-count-bounds.py`
