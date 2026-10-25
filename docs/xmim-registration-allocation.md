# XmIm registration allocation

XmIm keeps per-widget resource snapshots across input-method transport
restarts. Registration creation, value copies, and argument-array growth now
check allocation results before dereferencing or publishing new state.

Failed updates leave existing snapshots intact and skip only the resource that
could not be copied. Successful registrations retain their resource ordering
and restoration behavior.
