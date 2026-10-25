# String-table resource conversion

`XmRStringTable` now preserves an empty final field after a trailing comma,
matching its existing handling of interior empty fields. Positional resources
therefore retain all slots instead of silently shortening the table.
The converter also passes its checked initial, growth, and terminating table
sizes explicitly as Xt `Cardinal` byte counts. Individual fields are measured
in `size_t`, rejected before narrowing to the legacy `int` copy length, and
allocated with the checked span.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-string-table-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
