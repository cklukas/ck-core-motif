# Spinner interval rescheduling

When an active Spinner's `XmNspinnerInterval` changed, the old timeout stayed
armed until it fired once more.  The widget now removes that pending timeout
and schedules a new one immediately, so runtime interval changes take effect
without a stale tick.

`tests/automated/check-spinner-interval-reschedule.py` keeps the replacement
path in `SetValues`.
