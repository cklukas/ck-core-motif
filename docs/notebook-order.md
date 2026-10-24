# Notebook page ordering

Notebook children are sorted by page number before public page lookup. The
ordering comparator now uses relational comparisons, so the full signed `int`
page-number range remains ordered without arithmetic overflow. The regression
creates pages at `INT_MIN` and `INT_MAX` and verifies both through
`XmNotebookGetPageInfo`.

Run it against an installed prefix with:

```sh
python3 tests/automated/run-notebook-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```
