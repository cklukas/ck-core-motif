---
nav_exclude: true
---

# ColorSelector string getters

The ColorSelector synthetic string getters now check their internal label and
error strings before calling `XmStringCopy`. During partial initialization or
allocation failure they return a null value through the requested output slot
instead of dereferencing a missing compound string.

The focused source contract is checked by
`tests/automated/check-color-selector-string-getters.py`.
