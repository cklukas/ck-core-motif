---
nav_exclude: true
---
# Calendar geometry and callback guards

`XmCalendar` now handles missing drawing GCs, clamps selected-day geometry, and
avoids zero-width cell division in pointer handling. Callback routing compares
resource names by content, so activation and date-change callbacks remain
separate even when callers provide equivalent strings.

The focused source contract is checked by
`tests/automated/check-calendar-geometry-guards.py`.
