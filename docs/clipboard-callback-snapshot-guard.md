# Guard deferred clipboard snapshot callbacks

Deferred clipboard callbacks can arrive after their snapshot association has
already been removed during cancellation or widget teardown.  The callback
now checks its data identifier, dictionary, and snapshot record before
decrementing or dereferencing the record, returning cleanly when the
association is gone.

The focused source regression is
`tests/automated/check-clipboard-callback-snapshot-guard.py`.
