# XmString-table resource conversion

`XmRXmStringTable` now preserves an empty final field after a trailing comma,
matching its interior empty-field behavior. Positional compound-string tables
therefore retain all entries instead of silently shortening the table.

The converter also bounds table growth and the final NUL terminator allocation
against Xt's `Cardinal`-sized allocation interface. If a malformed or
unrepresentably large resource would overflow that interface, it releases the
already-created compound strings and reports conversion failure.

The source-level capacity regression is
`tests/automated/check-xmstring-table-growth.py`.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-xmstring-table-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
