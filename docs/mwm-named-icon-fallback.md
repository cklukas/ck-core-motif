# MWM named-icon fallback inputs

`MakeNamedIconPixmap()` now returns `None` for a missing client record or when
neither the client nor active MWM screen exists.  It skips named-file lookup
when the client has no screen record, then uses the selected active-screen
record for the built-in fallback.  Valid clients with screen data retain their
named pixmap, scaling, and default fallback behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-named-icon-fallback.py
```
