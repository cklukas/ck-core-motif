---
nav_exclude: true
---

# Value-control teardown guards

Calendar, date/time picker, toggle switch, skeleton, number box, spinner,
progress bar/ring, and rating control APIs now avoid live state after Xt begins
destroying their widget. Value queries return their existing empty result, and
the affected state-changing helpers become no-ops rather than updating child,
timer, or drawing state during teardown.

Normal control values, callbacks, and live-widget behavior are unchanged.
