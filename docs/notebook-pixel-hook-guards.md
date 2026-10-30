---
nav_exclude: true
---

# Notebook orientation hook guards

Notebook's horizontal/vertical pixel conversion hooks now validate the widget
class and callback value storage before reading orientation state. Invalid
exports return deterministic zero output; invalid imports return
`XmSYNTHETIC_NONE`. Valid Notebook subclasses retain their existing orientation
dependent conversion behavior.

The source-contract regression check is:

```sh
python3 tests/automated/check-notebook-pixel-hook-guards.py
```
