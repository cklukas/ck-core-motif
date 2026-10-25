---
nav_exclude: true
---
# I18List column-title allocation

I18List now copies column titles transactionally.  The title-pointer array is
checked against host `size_t` and Xt's `Cardinal` byte limits, and failures of
the array or an individual `XmStringCopy` release partial data and clear the
widget-owned pointer instead of dereferencing or publishing incomplete data.

The focused regression is
`tests/automated/check-i18list-column-title-allocation.py`.
