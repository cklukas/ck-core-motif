# MWM decoration allocation input guards

The frame-decoration setup helpers now reject invalid records before reading
their fields:

- `CountFrameRectangles()` and `InitClientDecoration()` return for a null
  screen record.
- `AllocateFrameDisplayLists()` returns `FALSE` for a null client record.

Normal screen and client records use the existing rectangle counting and
allocation behavior.  The guards protect startup and teardown callbacks from
turning a missing record into a window-manager crash.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-decoration-allocation-inputs.py
```
