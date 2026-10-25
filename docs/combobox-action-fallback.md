---
nav_exclude: true
---
# ComboBox action fallback

ComboBox action dispatch now treats an unknown direction or action value as a
safe no-op. The supported up/down, page, and data-navigation actions are
unchanged, while malformed action parameters no longer terminate the process.

The regression check is
`tests/automated/check-combobox-action-fallback.py`.
