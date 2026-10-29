# DataField position geometry

DataField caret-position conversion now keeps the right-aligned origin and
baseline arithmetic in signed 64-bit values before converting back to X11
`Position`. A small widget whose margins exceed its current allocation used
to wrap the unsigned width subtraction, producing a bogus caret coordinate.

Both axes now pass through the existing bounded position helper. Normal
alignment and offset behavior is unchanged, while transient resize states
remain inside the representable X11 coordinate range.

The source contract is checked by:

```text
python3 tests/automated/check-datafield-position-geometry.py
```
