# MWM icon source validation

`MakeClientIconPixmap()` and `MakeIconPixmap()` now reject missing source
pixmaps before geometry queries, and reject zero width/height inputs before
checking size limits, creating resources, or calling
`XCopyArea()`/`XCopyPlane()`. Valid icon scaling, masking, and beveling are
unchanged. A failed built-in bitmap allocation can no longer reach drawing
with an invalid source.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-source-validation.py
```
