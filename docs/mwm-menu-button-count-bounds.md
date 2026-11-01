---
nav_exclude: true
---
# MWM menu-button count bounds

MWM derives popup-menu button storage from configured menu-item lists. The
count now rejects an unrepresentable item before incrementing and verifies that
the final allocation fits Xt's `Cardinal` byte count. A pathological resource
fails cleanly without wrapping the count;
ordinary menu construction is unchanged.

`tests/automated/check-mwm-menu-button-count-bounds.py` protects this
boundary.
