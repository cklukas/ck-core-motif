# DataField string conversion safety

`XmDataFieldGetString` and `XmDataFieldSetStringWcs` now validate character
width and length arithmetic before allocating conversion buffers. They return
an empty/failure result safely when a buffer cannot be represented or
allocated, and a null wide input is treated as an empty string.

The regression check is:

```sh
python3 tests/automated/check-datafield-string-conversion.py
```
