---
nav_exclude: true
---
# ToggleSwitch label rollback

`XmToggleSwitch` now copies a changed label before releasing the current
`XmString`. Allocation failure preserves the existing label, while NULL still
explicitly clears it.

The focused source regression is
`tests/automated/check-toggle-switch-label-rollback.py`.
