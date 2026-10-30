---
nav_exclude: true
---
# Text-source accessor guards

Text-source value replacement and the editable, selection, and maximum-length
accessors now tolerate a null or partially destroyed source. Replacement
returns without changing state; boolean getters return `False`, the maximum
length getter returns zero, and setters become no-ops. Valid source behavior
is unchanged.

The focused source contract is checked by
`tests/automated/check-text-source-set-value-bounds.py`.
