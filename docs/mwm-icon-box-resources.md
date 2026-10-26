# MWM icon-box drawing resources

The icon-box GC helpers now handle resource failures safely:

- `MakeShrinkWrapIconsGC()` returns before `XCopyGC()` when its screen or GC is
  unavailable.
- `MakeFadeIconGC()` cleans up partial GCs and stipple pixmaps if any resource
  creation fails, then leaves the screen record disabled for retry.

Successful icon-box fade and shrink-wrap rendering is unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-box-resources.py
```
