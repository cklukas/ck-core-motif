# `XmUpdateDisplay` input contract

`XmUpdateDisplay` drains pending exposure events for the display associated
with a widget.  A null widget has no display and is not a valid target.  The
function now returns immediately before display lookup or application-context
setup when passed `NULL`, which makes cleanup and optional refresh paths safe.

The event-draining behavior for valid widgets is unchanged.
