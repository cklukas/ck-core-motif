---
nav_exclude: true
---

# XmList top-position conversion guards

The synthetic `XmNtopItemPosition` resource converts between Motif's public
one-based position and XmList's internal zero-based position. The conversion
callbacks now reject null output storage and unrelated widgets before touching
list state. Invalid exports return `XmINVALID_POSITION`; invalid imports leave
the requested value unchanged.

The source-contract regression check is:

```sh
python3 tests/automated/check-list-position-conversion-guards.py
```
