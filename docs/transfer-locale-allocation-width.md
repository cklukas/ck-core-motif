---
nav_exclude: true
---
# Locale-text allocation width

The locale-text conversion path bounds its accumulated string size below
Xt's `Cardinal` maximum before allocating. The allocation now passes that
validated size explicitly as `Cardinal`, preserving the checked contract on
32-bit and 64-bit builds.

The focused regression is
`tests/automated/check-transfer-locale-allocation-width.py`.
