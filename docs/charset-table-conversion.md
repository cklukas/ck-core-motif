# Charset-table resource conversion

`XmRCharSetTable` values are positional tables. Empty fields now remain
explicit `NULL` entries, so leading, repeated, and trailing commas preserve
alignment with related mnemonic and button tables instead of leaving pointers
uninitialized.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-charset-table-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
