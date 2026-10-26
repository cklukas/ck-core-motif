# Guard transfer callback context lookups

Secondary-selection completion now disowns the selection and returns cleanly
if its conversion context has disappeared.  Clipboard callback processing
also tolerates a missing context while preserving successful conversion
behavior.  This closes teardown races after a context allocation failure or
late widget destruction.

The focused source regression is
`tests/automated/check-transfer-callback-context-guards.py`.
