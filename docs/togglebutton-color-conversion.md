---
nav_exclude: true
---
# ToggleButton color-conversion fallback

ToggleButton SetValues paths now validate the payload returned by the
select-color converter. A missing or wrong-sized payload falls back to the
widget foreground; valid converter results retain their existing color.

The regression check is
`tests/automated/check-togglebutton-color-conversion.py`.
