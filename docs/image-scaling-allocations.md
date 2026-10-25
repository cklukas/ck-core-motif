---
nav_exclude: true
---
# Image scaling allocation failures

Scaled image rendering creates a temporary destination image and four
coordinate tables.  Previously, allocation failure left null pointers that
were immediately written through, turning resource exhaustion into a crash.

The scaler now initializes its table ownership, checks each allocation, and
releases already-created storage through the existing cleanup path.  The
`XImage` destination allocation is checked before any pixel work begins.
`tests/automated/check-image-scaling-allocations.py` protects this contract.
