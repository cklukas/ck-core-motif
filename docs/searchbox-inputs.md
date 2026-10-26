---
nav_exclude: true
---

# SearchBox value API inputs

The public SearchBox get, set, clear, and commit APIs now reject a null widget
before invoking the Xt subclass check. Invalid optional handles return the
existing `NULL` or no-op result; valid text-field updates and callback
behavior are unchanged.

The focused regression check is `tests/automated/check-searchbox-inputs.py`.
