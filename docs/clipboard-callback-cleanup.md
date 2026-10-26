---
nav_exclude: true
---

# Clipboard callback cleanup

When a clipboard conversion returns an invalid format, an overflowing size,
or a null value for non-empty data, `ClipboardCallback` now records a failed
copy with an empty clipboard payload and continues through the normal request
completion path.  The callback no longer returns before sending the final
`XmIDONE` conversion and freeing the snapshot request.

The regression check is
`tests/automated/check-clipboard-callback-cleanup.py`.
