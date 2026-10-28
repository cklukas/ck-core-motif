---
nav_exclude: true
---
# DataField clear-selection span

The clear-selection action now validates the selected character span before
narrowing it to the legacy `int` count used by replacement and temporary-space
buffers. Oversized spans are rejected without wrapping the count or allocating
an undersized buffer.

Regression coverage: `tests/automated/check-datafield-clear-selection-span.py`.
