---
nav_exclude: true
---

# Text selection-loss callback inputs

The Text and TextField selection-loss callbacks now reject a null widget, a
missing selection atom, or the `None` atom before deriving display state or
changing selection ownership. This makes owner replacement and destruction
callbacks safe during incomplete teardown.

The regression check is
`tests/automated/check-text-selection-loss-inputs.py`.
