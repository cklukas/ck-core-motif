# XmStringGenerate locking

`XmStringGenerate` now delegates synchronization to the parse, cache,
component and concatenation helpers instead of holding the process lock across
those calls. Rendition wrapping, fallback behavior and returned `XmString`
ownership remain unchanged.

The source contract is covered by
`tests/automated/check-xmstring-generate-locking.py`.
