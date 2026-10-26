# MWM frame-shape input guard

`SetFrameShape()` now returns immediately when its `ClientData` argument is
null.  This protects shape-notify and teardown callbacks before any border,
matte, or shaped-window fields are read; valid shaped and unshaped clients
retain the existing X Shape behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-frame-shape-inputs.py
```
