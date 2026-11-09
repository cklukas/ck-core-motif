---
nav_exclude: true
---
# PushButton default-frame reservation bounds

`XmPushButton` and `XmPushButtonGadget` reserve geometry for classic default
button emphasis. That reservation, the corresponding margin adjustments, text
origins, and recomputed widget sizes now use wide intermediates and saturate
at Motif's legacy `Position` and `Dimension` limits. This prevents a large
requested default shadow from wrapping a button or gadget into unrelated
geometry while preserving normal classic and modern profile behavior.

The focused source regression is
`tests/automated/check-pushbutton-reservation-bounds.py`.
