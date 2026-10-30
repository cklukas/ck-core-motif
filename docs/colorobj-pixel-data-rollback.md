# Color-object pixel-data rollback

`FetchPixelData` parses a server-provided pixel-set property before updating
the display color object.  Malformed input must not leave the selector
(`colorUse`) changed while the associated pixel fields are still old or
partially parsed.  The parser now publishes `colorUse` only after every color
field has validated and `UpdateXrm` has completed.

The focused source-contract check is:

```text
python3 tests/automated/check-colorobj-pixel-data-rollback.py
```
