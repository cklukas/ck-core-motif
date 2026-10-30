---
nav_exclude: true
---
# FontSelector encoding callback guards

Encoding callbacks now tolerate stale widget ancestry, incomplete font state,
missing encoding lists, and out-of-range menu indices. Valid menu selections
retain the existing encoding replacement and font-refresh behavior.
