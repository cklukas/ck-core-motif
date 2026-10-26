# MWM icon screen fallback

`MakeIconPixmap()` selects the client screen when available and otherwise uses
the active MWM screen.  It now returns `None` if both records are unavailable,
before reading icon-size limits or creating X resources.  Valid client and
active-screen records keep the existing icon scaling, masking, and beveling
behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-screen-fallback.py
```
