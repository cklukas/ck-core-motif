---
nav_exclude: true
---

# ColorPicker event guards

ColorPicker validates its widget type and event pointer before handling mouse
preview changes, keyboard commit/cancel, or channel adjustments. Invalid
dispatch is ignored safely while live-picker behavior remains unchanged.
