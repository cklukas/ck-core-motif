# XmString-table resource conversion

`XmRXmStringTable` now preserves an empty final field after a trailing comma,
matching its interior empty-field behavior. Positional compound-string tables
therefore retain all entries instead of silently shortening the table.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-xmstring-table-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
