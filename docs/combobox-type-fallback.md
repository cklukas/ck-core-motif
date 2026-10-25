---
nav_exclude: true
---
# ComboBox type fallback

ComboBox initialization now skips type-specific translation setup when the
internal type value is invalid. Shared accelerator installation still runs;
the documented combo-box, drop-down-list, and drop-down-combo-box modes are
unchanged.

The regression check is
`tests/automated/check-combobox-type-fallback.py`.
