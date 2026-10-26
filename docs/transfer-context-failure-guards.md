# Guard transfer-context lookup failures

Selection conversion, secondary-transfer, clipboard-source, drag-source, and
destination paths now handle a missing conversion context explicitly.  They
return a conversion failure or unwind their application lock instead of
dereferencing a null context during allocation pressure or teardown.

The focused source regression is
`tests/automated/check-transfer-context-failure-guards.py`.
