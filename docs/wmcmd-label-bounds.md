---
nav_exclude: true
---
# Mwm menu-info label bounds

Mwm’s menu-info replies keep labels in fixed protocol buffers. The previous
`strcpy` calls allowed a long resource-defined label to overwrite adjacent
state. All three menu-info paths now use bounded `snprintf`, retaining the
existing buffer sizes and safely truncating labels that do not fit; missing
labels become empty strings.

`tests/automated/check-wmcmd-label-bounds.py` guards these copies.
