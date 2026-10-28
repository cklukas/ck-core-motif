# Guard transfer-context lookup failures

Selection conversion, secondary-transfer, clipboard-source, drag-source, and
destination paths now handle a missing conversion context explicitly.  They
return a conversion failure or unwind their application lock instead of
dereferencing a null context during allocation pressure or teardown.

The focused source regression is
`tests/automated/check-transfer-context-failure-guards.py`.
Named selection ownership retains its current conversion context until
`XtOwnSelection` succeeds.  A rejected timestamp or other ownership failure
therefore leaves the existing owner able to service requests; the context is
replaced only after the server confirms the new owner.

`XmeSecondarySink` follows the same ordering for the destination selection, so
an unsuccessful ownership request cannot discard the active secondary-transfer
context.

`XmeClipboardSource` likewise retains its existing clipboard context while a
clipboard-manager ownership request is pending and replaces it only after Xt
confirms ownership.
