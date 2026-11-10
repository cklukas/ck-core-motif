# DataField insert-selection target

A client can ask a DataField that owns a selection to insert another client's
selection (the `INSERT_SELECTION` target). The DataField first asks that client
for its `TARGETS`, picks `COMPOUND_TEXT` or else `STRING` when they are offered,
and otherwise falls back to `TEXT`.

The fallback was read from a local variable before the variable had been
assigned, so a client that offered neither target made the DataField request an
arbitrary atom. `HandleInsertTargets` now sets the `TEXT` atom first and starts
from it.

`tests/automated/run-datafield-insert-target.py PREFIX RESULTS` compiles
`tests/automated/check-datafield-insert-target.c` against an installed
sanitized toolkit. A second X connection, served by its own thread, owns a
selection that offers only `UTF8_STRING`, asks the DataField to insert it and
records the target of the next request, which must be `TEXT`. The earlier
toolkit asked for an unrelated atom.
