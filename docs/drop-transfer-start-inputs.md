# Drop-transfer start inputs

`XmDropTransferStart` now requires an `XmDragContext`, rejects a nonzero
argument count with a null argument list, and checks both the display object
and its drop-transfer class before dispatching the start operation. Invalid
construction state returns `NULL` without touching Xt state or dereferencing a
missing class method.

Regression coverage: `tests/automated/check-drop-transfer-start-inputs.py`.
