---
nav_exclude: true
---
# XmString multibyte conversion

`_XmGetMBStringFromXmString` now sizes its output with `size_t` and copies
components by length rather than repeatedly concatenating into an allocation
calculated with a signed `int`. Component-length overflow is rejected before
allocation, while ordinary compound-string conversion remains unchanged.
