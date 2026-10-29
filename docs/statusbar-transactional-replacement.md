---
nav_exclude: true
---
# StatusBar item replacement

`XmStatusBarSetItems` now builds and validates the replacement item array and
its label children before releasing the current presentation.  Allocation,
label-construction, or child-creation failure therefore leaves existing items
visible and usable instead of clearing the status bar.

The contract is protected by
`tests/automated/check-statusbar-transactional-replacement.py`.
