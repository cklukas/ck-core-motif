---
nav_exclude: true
---
# FontSelector encoding matching guards

Encoding matching now handles missing selector/family state and an unset
encoding safely. It checks the family encoding capacity before indexing, so
malformed or truncated encoding arrays cannot be read out of bounds.
