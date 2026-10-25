# XmString conversion guard

`WmXmStringToString` now checks `XmStringInitContext` before reading segments
or freeing the context.  Invalid or uninitializable compound strings return
`NULL` safely; valid strings retain the existing conversion behavior.

`tests/automated/check-xmstring-conversion-guards.py` protects the contract.
