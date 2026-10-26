---
nav_exclude: true
---
# TextField insertion conversion lengths

External TextField insertion now rejects lengths that cannot fit its legacy
integer path or wide-character allocation, and handles failed temporary
allocations before copying or converting selection data.

Regression coverage: `tests/automated/check-textfield-insert-length.py`.
