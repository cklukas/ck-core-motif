---
nav_exclude: true
---
# Link resource and drawing safety

`XmLink` now skips redisplay until its window and private GC are available.
Label and target replacements release old owned values only after successful
copies; failed copies retain the prior values. Valid rendering, activation,
and resource behavior are unchanged.

`tests/automated/check-link-resource-safety.py` protects these paths.
