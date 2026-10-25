# Icon-place lookup guards

`GetNextIconPlace` now returns `NO_ICON_PLACE` when placement storage is
missing or empty.  The forward position conversion also rejects indices past
the placement list, avoiding invalid reads while preserving its zero-position
fallback.

`tests/automated/check-next-place-guards.py` protects these bounds.
