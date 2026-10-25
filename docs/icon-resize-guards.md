# Icon-box resize guards

`CheckIconBoxResize` now ignores events received before the icon-box state is
complete or when width/height increments are invalid.  This prevents null
dereferences and divide-by-zero during resize handling while preserving normal
resize behavior.

`tests/automated/check-icon-resize-guards.py` protects this path.
