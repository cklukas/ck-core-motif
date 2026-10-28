---
nav_exclude: true
---
# Text target-discovery failure completion

`HandleTargets` now completes transfers with failure when target data is
malformed, the insertion overlaps the current selection, or follow-up state
cannot be allocated. These paths no longer leave an external selection request
pending after freeing its callback data.

Regression coverage: `tests/automated/check-textsel-handle-targets-failure.py`.
