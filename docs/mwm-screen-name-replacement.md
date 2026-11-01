---
nav_exclude: true
---
# MWM screen-name replacement safety

MWM now stages replacement allocations for its generated per-screen names and
the temporary workspace-name pointer array. A failed `XtRealloc` leaves the
published screen name intact; workspace parsing releases only its temporary
pointer array and follows the controlled window-manager error exit. This also
avoids an unnecessary workspace-array growth after the final accepted name.

Valid configurations retain their existing screen and workspace names and
ordering. The change only makes allocation failure deterministic if an error
exit is intercepted by an embedding or test harness.

`tests/automated/check-mwm-screen-name-replacement.py` protects both paths.
