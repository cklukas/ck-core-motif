---
nav_exclude: true
---
# TextField target-discovery failure completion

TextField target discovery now completes transfers with failure when target
data is malformed, the insertion overlaps the current selection, or follow-up
state cannot be allocated. These paths no longer leave a request pending after
freeing its callback data.

Regression coverage: `tests/automated/check-textfsel-handle-targets-failure.py`.
