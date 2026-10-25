---
nav_exclude: true
---
# LabelG set-values allocation handling

`SetValuesPrehook` prepares replacement and request-side cache extension objects
before applying resource changes.  Both extension-object allocations and the
widget extension-data record are now checked.  If allocation fails, any
partially created objects are released and the hook leaves the existing cache
untouched, preserving the normal `FALSE` set-values result without dereferencing
null state.

The regression check is `tests/automated/check-labelg-setvalues-allocation.py`.
