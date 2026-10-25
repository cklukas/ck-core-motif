---
nav_exclude: true
---

# Form-section margin comparison

`XmFormSection` now treats the available inner width as zero when margins are
larger than the natural width. Footer sizing therefore expands the section when
needed instead of relying on an unsigned subtraction that could wrap around.

The focused regression check is
`tests/automated/check-form-section-margin-bounds.py`.
