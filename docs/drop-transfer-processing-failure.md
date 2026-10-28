# Drop-transfer processing failures

Drop-transfer processing now fails closed when a transfer list is empty or
when temporary target/client-data arrays cannot be allocated.  The temporary
arrays are released, the transfer status is set to `XmTRANSFER_FAILURE`, and
the source is notified through the normal termination path.  This prevents a
drop object from waiting forever without a callback.

Regression coverage: `tests/automated/check-drop-transfer-processing-failure.py`.
