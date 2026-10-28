---
nav_exclude: true
---

# XPM data extension write status

Extension serialization now returns a status. The data builder checks its
formatted extension records and rejects missing metadata or formatting
failures instead of silently returning partially written data.

Regression: `tests/automated/check-xpm-data-extension-write-status.py`
