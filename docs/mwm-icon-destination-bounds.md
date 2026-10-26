# MWM icon destination bounds

`MakeIconPixmap()` now rejects zero maximum icon dimensions and checks that
shadow/slab margins fit in unsigned arithmetic before constructing the
destination pixmap size.  Valid icon dimensions retain the existing margin,
scaling, and bevel behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-icon-destination-bounds.py
```
