---
nav_exclude: true
---

# Transfer status input validation

`XmTransferDone` now rejects values outside the four public
`XmTransferStatus` enumerators before changing queue state or sending a
selection request. Valid `SUCCEED`, `FAIL`, `CONTINUE`, and `DEFAULT` calls
retain their existing semantics; malformed enum values fail closed.

The focused source-contract check is
`tests/automated/check-transfer-context-inputs.py`.
