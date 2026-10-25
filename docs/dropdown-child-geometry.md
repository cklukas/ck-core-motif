---
nav_exclude: true
---
# DropDown child geometry

DropDown child placement subtracts vertical spacing, shadow, borders, and
label/arrow widths from unsigned dimensions. During a collapsed resize those
subtractions could wrap, producing oversized child heights or unrelated
horizontal positions.

Placement now uses signed intermediates, clamps the available child height, and
saturates each child span after its border inset. Normal layouts retain their
existing geometry while transiently tiny controls remain bounded.
