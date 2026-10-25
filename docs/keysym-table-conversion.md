# KeySym-table resource conversion

`XmRKeySymTable` values are positional tables. Empty fields now remain
`NoSymbol` entries, preserving alignment with related mnemonic and accelerator
tables instead of leaving keysyms uninitialized when a resource contains
leading, repeated, or trailing commas.

The converter also checks the comma count against both host `size_t` and Xt
`Cardinal` byte limits before allocating the table, so an unrepresentable
resource length cannot wrap `sizeof(KeySym) * (count + 2)`.
The capacity regression is
`tests/automated/check-keysym-table-bounds.py`.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-keysym-table-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
