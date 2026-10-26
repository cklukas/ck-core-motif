---
nav_exclude: true
---
# Render-table token bounds

The render-table parser keeps its public position interface as an `int` for
compatibility, but token scanning now rejects positions and token lengths that
would overflow that type.  This prevents signed-overflow undefined behavior
for malformed or extremely large render-table strings while preserving normal
tokenization and the existing end-of-input behavior.

`tests/automated/check-render-token-bounds.py` protects the overflow guards.
