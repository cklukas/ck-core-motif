# DataField allocation safety

The legacy `XmDataField` implementation shares the TextField validation and
modify-verify patterns used by older applications. Its narrow and wide value
paths now check allocations before copying, and veto restoration keeps the
`XtRealloc` result in a temporary pointer before copying. Allocation failure
therefore cannot turn into a null write or corrupt the previous value.

The regression check is:

```sh
python3 tests/automated/check-datafield-allocation.py
```
