---
nav_exclude: true
---
# DatePicker popup positioning

DatePicker popup positioning now treats a popup wider or taller than its
screen as an oversized case and anchors that axis at zero.  Normal right and
bottom edge clamping is unchanged, while small-screen layouts no longer
receive negative coordinates.

`tests/automated/check-datepicker-popup-position.py` protects this geometry.
