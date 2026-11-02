---
nav_exclude: true
---
# MWM desktop-request queue ownership

`_DT_WM_REQUEST` is a property on each screen's workspace window. MWM now
keeps a partially consumed, NUL-separated request property in that screen's
`WmScreenData`, rather than in function-static storage shared by every
screen. Concurrent desktop requests on multiple screens therefore cannot
continue or free each other's queued data.

If allocating the next private request copy fails, MWM releases that screen's
queued property and stops the current drain. This avoids a repeated
allocation-failure loop while leaving future property notifications usable.
