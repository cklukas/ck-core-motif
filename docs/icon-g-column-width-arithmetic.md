---
nav_exclude: true
---

# IconG column-width arithmetic

IconG now normalizes detail-column width with signed `long long` intermediates.
RTL parent-width subtraction no longer wraps through unsigned `Dimension`
arithmetic, and comparisons no longer narrow the column width to signed
`Position`.  Non-positive available space uses the existing minimum-width
fallback.

`tests/automated/check-icon-g-column-width-arithmetic.py` protects this
normalization contract.
