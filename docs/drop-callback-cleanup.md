---
nav_exclude: true
---
# Drop callback cleanup

Drop completion cleanup now clears `destination_data` immediately after freeing
it. Repeated completion or cancellation callbacks therefore become harmless
instead of double-freeing the same drop allocation.

Regression coverage: `tests/automated/check-drop-callback-cleanup.py`.
