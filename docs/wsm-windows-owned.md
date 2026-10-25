---
nav_exclude: true
---

# WSM window-request ownership

`WSM_WM_WINDOWS` request decoding owns its window-property and match-attribute
arrays through the `wm_windows.allocated` flag.  The decoder now sets that
field directly instead of marking the unrelated extensions state, so normal
request cleanup can release the arrays after use.
