# DataField timer edge bounds

DataField autoscroll now computes its text edges in signed `int64_t` values.
During a resize, the widget's margins can temporarily exceed its allocation;
subtracting those margins from an unsigned X11 `Dimension` previously wrapped
to a large positive coordinate. The timer could then scroll in the wrong
direction or store an invalid selection position.

The edge results are clamped through a local `Position` helper before they are
stored, and a missing timer event is rejected without dereferencing it.

The source contract is checked by:

```text
python3 tests/automated/check-datafield-timer-edge-bounds.py
```
