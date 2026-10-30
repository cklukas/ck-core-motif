---
nav_exclude: true
---

# LabelGadget padding hook guards

LabelGadget's orientation-dependent padding converters now validate widget and
value inputs before reading pixmap placement or forwarding unit conversion.
Invalid imports return `XmSYNTHETIC_NONE`; invalid exports return without
touching caller storage.

The focused source-contract check is:

```sh
python3 tests/automated/check-labelg-padding-hook-guards.py
```
