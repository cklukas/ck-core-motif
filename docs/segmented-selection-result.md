# SegmentedControl selection result

`XmSegmentedControlSetSelectedIndex` now returns whether the requested index
is the actual resulting selection.  A clear request (`-1`) is correctly
reported as rejected when `allow_clear` is disabled, instead of always
returning success.

The focused source-contract check is:

```sh
python3 tests/automated/check-segmented-selection-result.py
```
