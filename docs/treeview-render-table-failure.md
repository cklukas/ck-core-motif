# TreeView render-table replacement failures

`XmTreeView` owns a copy of the render table used for row labels. During a
resource update it copies the requested table before releasing the old one.
If the copy cannot be allocated, the existing valid table remains installed so
the tree stays drawable and can recover on a later update.

The focused source contract is checked with:

```sh
python3 tests/automated/check-treeview-text-metrics.py
```
