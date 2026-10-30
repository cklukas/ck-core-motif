# Color-object property trailing data

The ColorObj server property has a fixed sequence of underscore-terminated
hexadecimal fields. After parsing the selector and every pixel-set field,
`FetchPixelData` now requires the cursor to be at the terminating NUL. Trailing
bytes are rejected before any color state is published.

The focused source-contract check is:

```text
python3 tests/automated/check-colorobj-property-trailing-data.py
```
