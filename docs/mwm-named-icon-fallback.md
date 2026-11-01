# MWM named-icon fallback inputs

`MakeNamedIconPixmap()` now returns `None` for a missing client record or when
neither the client nor active MWM screen exists.  It skips named-file lookup
when the client has no screen record, then uses the selected active-screen
record for the built-in fallback.  Valid clients with screen data retain their
named pixmap, scaling, and default fallback behavior.

The local named-pixmap result begins as `XmUNSPECIFIED_PIXMAP`. Therefore a
client with a valid screen but no icon name takes the built-in icon fallback
instead of testing an uninitialized X resource ID.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-named-icon-fallback.py
```
