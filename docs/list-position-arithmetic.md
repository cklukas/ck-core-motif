---
nav_exclude: true
---
# List position arithmetic

List add and delete APIs now validate one-based positions before converting
them to zero-based indexes. Negative add positions retain the historical
append behavior without evaluating an overflowing subtraction; delete
positions at or below zero are rejected. Delete counts are bounded with
subtraction instead of overflowing index-plus-count arithmetic.

Regression coverage: `tests/automated/check-list-position-arithmetic.py`.
