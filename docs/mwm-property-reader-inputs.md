# MWM property-reader inputs

`GetNormalHints` now returns its existing default-hints structure when called
without a client record, and `GetMwmHints` returns `NULL` before accessing the
client. This makes both property readers safe for defensive callers while
preserving their normal X-property behavior.

The focused source contract is covered by
`tests/automated/check-mwm-property-reader-inputs.py`.
