---
nav_exclude: true
---

# Text callback conversion widths

Modify-verify callback buffers now use explicit `size_t` copy lengths and pass
the checked wide-character buffer size through Xt's `Cardinal` boundary.
`mbstowcs` results are validated before narrowing to the legacy callback block
length, with invalid conversion retaining the existing empty-result behavior.

Regression: `tests/automated/check-text-callback-conversion-width.py`
