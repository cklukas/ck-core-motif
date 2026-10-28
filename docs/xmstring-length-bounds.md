# XmString length bounds

`XmStringLength()` has a public signed-`int` return type, while its internal
byte-stream conversion reports an unsigned length.  The helper now clamps
unrepresentable lengths to `INT_MAX` before returning, preventing overflow for
large compound strings and giving List, ComboBox, and SelectionBox callbacks a
stable ABI value.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-xmstring-length-bounds.py
```
