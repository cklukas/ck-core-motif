---
nav_exclude: true
---

# MultiList selection API inputs

MultiList selection, deselection, visibility, and selected-row query helpers
now reject a null widget before entering the application context. Array
queries return `NULL` (and reset the optional row count); mutators remain
no-ops. Valid selection behavior is unchanged.

The focused regression check is `tests/automated/check-multilist-inputs.py`.
