---
nav_exclude: true
---
# Text destination callback events

Text and TextField destination callbacks now reject a missing event pointer
before storing it in the shared insertion context. If a transfer ID exists,
the malformed transfer is explicitly failed rather than left pending.

Regression coverage: `tests/automated/check-text-destination-event.py`.
