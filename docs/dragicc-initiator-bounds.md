---
nav_exclude: true
---

# Drag ICC initiator metadata bounds

Initiator metadata now rejects a target-table index above the ICC property's
16-bit wire range before serialization. Xt argument counters use `Cardinal`,
and the ICC handle conversion is explicit at the 32-bit property boundary.

Regression: `tests/automated/check-dragicc-initiator-bounds.py`
