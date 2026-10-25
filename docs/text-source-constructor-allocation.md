# Text source constructor allocation

`_XmStringSourceCreate` now treats source construction as a transaction.  A
failed source record, data record, text buffer, wide-input conversion buffer,
newline buffer, or widget array releases every earlier allocation and returns
`NULL`.  A null wide input is treated as an empty value, matching the existing
narrow-input behavior instead of being dereferenced.

The source regression in `tests/automated/check-text-source-constructor-allocation.py`
protects the allocation checks, cleanup label, and null-input handling.
