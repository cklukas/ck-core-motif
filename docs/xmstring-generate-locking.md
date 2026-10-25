# XmStringGenerate locking

`XmStringGenerate` now delegates synchronization to parse, cache, component,
and concatenation helpers instead of holding the process lock across those
calls. Rendition wrapping and fallback behavior remain unchanged.
