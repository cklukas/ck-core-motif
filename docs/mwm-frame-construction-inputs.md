# MWM frame construction input guards

`ConstructFrame()` now returns `FALSE` when its `ClientData` argument is null,
and `GenerateFrameDisplayLists()` returns without drawing.  Both routines are
used during frame setup and redraw; rejecting an invalid lifecycle callback
keeps the window manager alive while preserving the existing behavior for a
valid client record.

The source-contract regression check is:

```sh
python3 tests/automated/check-mwm-frame-construction-inputs.py
```
