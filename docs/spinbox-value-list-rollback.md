---
nav_exclude: true
---
# SimpleSpinBox value-list rollback

`XmSimpleSpinBoxAddItem` and `XmSimpleSpinBoxDeletePos` now build temporary
value arrays transactionally.  Each `XmStringCopy` is checked and partial
copies are released on failure; the child’s values and current position are
updated only after the complete array is ready.  Allocation sizes are bounded
against both host `size_t` and Xt’s `Cardinal` range.

The focused regression is
`tests/automated/check-spinbox-value-list-rollback.py`.
