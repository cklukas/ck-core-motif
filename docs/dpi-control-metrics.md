---
nav_exclude: true
---
# WP-04 control-metric audit

`tests/automated/check-dpi-control-metrics.c` is an installed consumer that
records default metrics outside the already-qualified indicator and SpinBox
subset.  It creates real PushButton, TextField, ScrollBar, Notebook and
MessageDialog controls at 75, 150 and 300 `Xft.dpi`, then realizes the
hierarchy under Xvfb.

The fixture records focus-ring and shadow thickness, text margins, scrollbar
dimensions, Notebook frame/back-page metrics, dialog margins, and a real
TextField selection interval.  It also checks that an explicit PushButton
highlight thickness and an explicit ScrollBar width remain unchanged when the
controls are realized outside a parent-owned layout policy.

The current AArch64 `uconsole` observation is:

| DPI | Push highlight/shadow | Text highlight/margin | Scrollbar width/height | Notebook frame/back page | Dialog margin |
| ---: | ---: | ---: | ---: | ---: | ---: |
| 75 | 2 / 2 | 2 / 5 | 160 / 104 | 2 / 8 | 10 |
| 150 | 2 / 2 | 2 / 5 | 192 / 104 | 2 / 8 | 10 |
| 300 | 2 / 2 | 2 / 5 | 256 / 104 | 2 / 8 | 10 |

The values are audit evidence, not universal defaults for every X server or
resource database.  In particular, focus/shadow/text/dialog metrics remain
constant in this build while the default ScrollBar width changes; this is a
classification result, not an automatic correction.  Explicit application
values and selection state are preserved.  No source change is justified by
this fixture alone; any future correction must identify the intended contract
and compare actual windows on the supported display cohort.
