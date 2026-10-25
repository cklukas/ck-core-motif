---
nav_exclude: true
---
# ScrollFrame navigator removal input validation

`_XmSFRemoveNavigator` now rejects a null navigator or scroll-frame state
before querying the navigator trait or changing callbacks.  This keeps the
convenience API from dereferencing invalid pointers during teardown and
partial initialization.

The regression check is
`tests/automated/check-scrollframe-remove-input.py`.
