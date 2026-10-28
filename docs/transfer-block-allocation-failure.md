# Transfer-block allocation failure

When a destination transfer cannot allocate its next `TransferBlock`, the
destination now reports failure through the normal completion path.  The
transfer context and any callback-owned destination data are released instead
of returning with an unobservable, half-open transfer.

Regression coverage: `tests/automated/check-transfer-block-allocation-failure.py`.
