---
nav_exclude: true
---
# I18List pixmap conversion allocation guard

I18List drag conversion now validates the active drag context and the
temporary `Pixmap` result allocation.  Allocation failure reports
`XmCONVERT_REFUSE` without dereferencing NULL or returning an incomplete
conversion result.

The focused regression is
`tests/automated/check-i18list-pixmap-conversion-allocation.py`.
