# SegmentedControl selection preservation

`XmSegmentedControlSetItems` now preserves the selected segment by comparing
item IDs by string content while the old item array is still live.  The old
implementation compared pointers after freeing the old array, which both lost
selection during item replacement and read a dangling pointer.

The focused source-contract check is:

```sh
python3 tests/automated/check-segmented-selection-preservation.py
```
