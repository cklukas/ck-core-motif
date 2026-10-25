---
nav_exclude: true
---
# DialogShell count bounds

DialogShell unmanage cleanup now uses Xt `Cardinal` counters for both the
child popup list and each nested shell's composite child list. Popup cleanup
ordering is unchanged while signed-counter wraparound is avoided for large
valid lists.
