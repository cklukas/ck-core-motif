---
nav_exclude: true
---
# Text TARGETS allocation failure

Text selection conversion obtains a base target array before appending the
formats supported by the current selection.  The returned array is now
checked before it is extended.  If allocation fails, conversion reports a
null value and refuses the request without writing through invalid storage.

The regression check is
`tests/automated/check-textsel-targets-allocation.py`.
