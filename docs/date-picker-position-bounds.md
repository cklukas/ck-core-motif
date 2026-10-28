---
nav_exclude: true
---

# DatePicker popup positioning

DatePicker computes popup coordinates in wide intermediates and clamps them to
the target screen before converting to X11 `Position` values. Large or
negative owner coordinates can no longer wrap and misplace the popup; the
final coordinates are also bounded by the signed `Position` representation.
