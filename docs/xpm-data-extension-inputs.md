---
nav_exclude: true
---

# XPM data extension inputs

The data serializer now validates extension output pointers, the extension
array, names, line arrays, and individual lines before measuring them. Empty
extension sets remain valid and produce zero size/count outputs.

Regression: `tests/automated/check-xpm-data-extension-inputs.py`
