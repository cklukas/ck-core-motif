# MWM icon source validation

`MakeIconPixmap()` now rejects `None` source pixmaps and zero width/height
inputs before checking size limits, creating resources, or calling
`XCopyArea()`/`XCopyPlane()`.  Valid icon scaling, masking, and beveling are
unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-source-validation.py
```
