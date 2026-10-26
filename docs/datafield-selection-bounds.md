# DataField selection bounds

`XmDataFieldGetSelection` now checks selection length and character-width
arithmetic before converting the result to Xt's `Cardinal` allocation size.
Oversized or malformed ranges return `NULL` without wrapping the allocation
size, while normal narrow and multibyte selections retain their existing
behavior.

The regression check is:

```sh
python3 tests/automated/check-datafield-selection-bounds.py
```
