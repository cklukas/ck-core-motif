# NumberBox render-table replacement failures

`XmNumberBox` owns a copy of the render table used for its value text. During
resource updates it copies the requested table before releasing the old one.
If the copy cannot be allocated, the existing valid table remains installed so
the control stays drawable and can recover on a later update.

The focused source contract is checked with:

```sh
python3 tests/automated/check-numberbox-font-fallback.py
```
