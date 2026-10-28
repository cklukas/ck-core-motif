---
nav_exclude: true
---

# DragUnder argument-count width

`CreateAnimationSaveData` now tracks its `XtSetArg`/`XmDropSiteRetrieve`
argument count as `Cardinal`, matching Xt's API and avoiding a signed-to-
unsigned conversion at the retrieval call.

Regression: `tests/automated/check-dragunder-arg-count.py`
