# CommandPalette item-array bounds

`XmCommandPaletteSetItems` now validates the requested item count before
allocating stored command records.  The multiplication must fit both native
`size_t` arithmetic and Xt's `Cardinal` allocation width; oversized requests
return with a warning before any item records are accessed.

The focused source-contract check is:

```sh
python3 tests/automated/check-command-palette-allocation-bounds.py
```
