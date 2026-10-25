---
nav_exclude: true
---
# MWM CCI label prefixing

Legacy CCI modifiers prepend `->`, `=`, `=>`, or `~` to a menu label.  The
prefix operation now checks the combined size, allocates one replacement, and
swaps it only after the copy succeeds.  The original label remains intact if
allocation fails, and valid modifier behavior is unchanged.

`tests/automated/check-mwm-cci-label-prefix.py` protects this contract.
