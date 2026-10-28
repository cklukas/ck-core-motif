---
nav_exclude: true
---

# XIM preedit conversion

The XIM preedit bridge now handles `mbstowcs` failure explicitly and requires
the conversion to produce exactly the number of wide characters advertised by
the input method. Invalid locale text is rejected before the replacement
buffer is published, so a failed preedit update cannot leave partially
converted text in the widget.

Regression check:

```text
python3 tests/automated/check-xmim-preedit-conversion.py
```
