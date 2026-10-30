---
nav_exclude: true
---

# SimpleSpinBox synthetic getter guards

The shared SimpleSpinBox synthetic getter now initializes deterministic output
and validates the output pointer, widget class, and text-field child before
forwarding resource reads. Calls made during partial construction or with an
unrelated widget no longer dereference invalid state; valid resource exports
remain unchanged.

The source-contract regression check is:

```sh
python3 tests/automated/check-simplespinbox-getter-guards.py
```
