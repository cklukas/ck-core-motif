---
nav_exclude: true
---
# MWM gadget hit-test array validation

`GadgetID` returns `FRAME_NONE` when its gadget-array pointer is null.  This
keeps malformed hit-test requests safe even when their count is nonzero;
normal iteration and gadget ID results are unchanged for valid arrays.

The source-contract regression is
`tests/automated/check-mwm-gadget-array-inputs.py`.
