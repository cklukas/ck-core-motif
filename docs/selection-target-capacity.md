# Selection TARGETS capacity and failure handling

`XmeStandardTargets` takes the number of private target atoms that its caller
will append. A converter must reserve its maximum conditional set, not only
the atoms that happen to be present in the most common locale or drag state.

`XmList` now reserves its optional `XA_STRING` target, while `XmI18List`
reserves both optional `XA_STRING` and `XA_PIXMAP` targets. This keeps their
advertised TARGETS lists inside the allocation for every encoding and drag
icon configuration. The I18List export path and the maintained transfer
examples also decline a conversion cleanly when their target-array allocation
fails.

The focused source-contract regression is:

```sh
python3 tests/automated/check-selection-target-capacity.py
```
