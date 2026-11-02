---
nav_exclude: true
---

# MWM transient-property refresh

MWM reprocesses `WM_TRANSIENT_FOR` when a client changes or removes the
property.  A previous managed leader is now cleared before the replacement
target is resolved.  If the new property is a self-reference or names a window
that MWM does not currently manage, the client retains the transient hint but
does not remain linked, stacked, or modalized below its former leader.

Before a managed transient or pseudo-transient window-group relationship is
linked, MWM also rejects a direct or indirect path back to the client (and an
already cyclic leader chain). A malformed pair of changing client hints cannot
therefore create a transient-tree loop that blocks focus or restacking.

The focused source regression is:

```sh
python3 tests/automated/check-mwm-transient-property-reset.py
```
