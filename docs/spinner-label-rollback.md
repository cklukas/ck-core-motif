---
nav_exclude: true
---
# Spinner label rollback

`XmSpinner` now copies a changed label before releasing the current
`XmString`. Allocation failure preserves the existing label, while NULL still
explicitly clears it.

The focused source regression is
`tests/automated/check-spinner-label-rollback.py`.
