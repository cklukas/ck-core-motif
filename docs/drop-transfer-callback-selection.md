# Drop-transfer callback selection validation

The drop-transfer selection callback now rejects a null selection atom before
dispatching to application transfer code. Xt supplies a real selection atom
for a valid request; `None` indicates malformed or stale callback data and
cannot complete a transfer. Valid callbacks retain their existing incremental
and multi-target behavior.

Regression coverage: `tests/automated/check-drop-transfer-callback-selection.py`.
