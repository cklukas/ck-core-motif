# Cardinal-list resource conversion

`XmRCardinalList` now parses values with range checks. Values above the native
`Cardinal` maximum, negative values, and malformed suffixes are rejected rather
than silently wrapped or skipped; valid comma- and whitespace-separated values
retain the existing representation.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-cardinal-list-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
