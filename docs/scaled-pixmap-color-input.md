# Scaled pixmap color input validation

`_XmGetScaledPixmap` now rejects a missing `XmAccessColorData` record before
cache lookup or image construction. The helper later copies the record and
uses its color fields, so accepting a null pointer previously caused a crash
through a private toolkit boundary. Existing callers that provide color data
retain their behavior; invalid input now returns `XmUNSPECIFIED_PIXMAP`.

The focused regression is
`tests/automated/check-xm-scaled-pixmap-color-input.py`.
