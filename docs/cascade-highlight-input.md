---
nav_exclude: true
---
# Cascade-gadget highlight null-widget guard

`XmCascadeButtonGadgetHighlight` now returns immediately for a null widget,
before application-context lookup. This prevents an invalid highlight request
from crashing the caller while preserving the existing behavior for valid
cascade-button gadgets.

The focused regression is
`tests/automated/check-cascade-highlight-input.py`.
