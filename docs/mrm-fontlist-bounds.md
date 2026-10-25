---
nav_exclude: true
---
# Mrm legacy font-list bounds

Legacy Mrm 1.1 font-list conversion checks the item count before evaluating
`count - 1` and allocating the compatibility record. Zero, negative, or
unrepresentable counts fail without underflow; valid font lists retain their
existing conversion behavior.

The source-level regression is `tests/automated/check-mrm-fontlist-bounds.py`.
