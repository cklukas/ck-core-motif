---
nav_exclude: true
---
# ScrollFrame navigator update guards

`_XmSFUpdateNavigatorsValue` now checks its scroll-frame and navigator traits,
the returned navigator list, and each navigator entry before invoking callback
methods.  Partially initialized or tearing-down widgets are skipped safely.

The regression check is
`tests/automated/check-scrollframe-update-guards.py`.
