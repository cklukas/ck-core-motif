---
nav_exclude: true
---

Guard typeless conversion results
=================================

`_XmConvertHandler` now rejects a conversion result that reports a non-empty
payload with `type == None`.  X selection data must identify its property type;
otherwise consumers may interpret an otherwise valid pointer using an
undefined atom.  The handler still accepts empty results and preserves the
existing 8/16/32-bit format checks.  Rejected results are returned as an
unsuccessful conversion with no published payload.

The focused regression check is:

```text
python3 tests/automated/check-transfer-convert-result.py
```
