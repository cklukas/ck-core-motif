---
nav_exclude: true
---

# ScrollFrame navigator callback removal

Navigator removal now checks that the navigator trait provides
`changeMoveCB` before invoking it. A partially implemented trait no longer
causes a null function-pointer call; the navigator is still removed from the
ScrollFrame list.

The focused regression check is
`tests/automated/check-scrollframe-remove-callback.py`.
