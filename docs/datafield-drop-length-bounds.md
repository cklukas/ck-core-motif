---
nav_exclude: true
---
# DataField drop-transfer length bounds

DataField drop transfers can arrive with an X11 selection length wider than
the widget's legacy `int` text API.  The callback now accumulates converted
payload sizes in `size_t`, rejects values that cannot be represented by the
DataField API, and checks the wide-character allocation and conversion result
before replacing text. Ordinary drops retain their existing behavior.

`tests/automated/check-datafield-drop-length-bounds.py` is the focused source
regression for this boundary.
