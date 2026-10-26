# MWM named-icon fallback inputs

`MakeNamedIconPixmap()` now returns `None` for a missing client record and
skips named-file lookup when the client has no screen record.  In the latter
case the existing built-in icon fallback is used through `MakeIconPixmap()`.
Valid clients with screen data retain their named pixmap, scaling, and default
fallback behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-named-icon-fallback.py
```
