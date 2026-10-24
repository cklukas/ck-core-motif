# KeySym-table resource conversion

`XmRKeySymTable` values are positional tables. Empty fields now remain
`NoSymbol` entries, preserving alignment with related mnemonic and accelerator
tables instead of leaving keysyms uninitialized when a resource contains
leading, repeated, or trailing commas.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-keysym-table-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
