# Drop-transfer construction allocation

`StartDropTransfer` now checks the result of `XtCreateWidget` before storing
drag state or scheduling its timer.  A failed object allocation returns `NULL`
through the existing `XmDropTransferStart` failure contract instead of
dereferencing an unavailable transfer object.

Regression coverage: `tests/automated/check-drop-transfer-start-allocation.py`.
