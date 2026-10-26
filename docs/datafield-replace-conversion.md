# DataField replace conversion safety

The public DataField replace APIs now check their temporary wide/multibyte
conversion buffers before use. Invalid wide input is represented by an owned
empty buffer, and conversion failure clears that buffer in place; cleanup no
longer attempts to free a string literal.

The regression check is:

```sh
python3 tests/automated/check-datafield-replace-conversion.py
```
