---
nav_exclude: true
---
# Final transfer teardown owns drop cleanup

Drop transfers allocate a private copy of their callback data. The usual done
callback releases that copy, but callback-list growth can fail under memory
pressure. `FinishTransfer` now performs the same idempotent release after all
callbacks, so the transfer context remains the authoritative owner and cannot
leak the drop data when callback registration fails.

Regression coverage: `tests/automated/check-finish-transfer-drop-cleanup.py`.
