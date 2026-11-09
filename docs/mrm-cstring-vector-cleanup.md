# Mrm compound-string vector cleanup

Mrm converts every serialized compound-string vector entry independently.  If
one malformed entry fails conversion, it now frees the entries converted before
it and returns `MrmFAILURE`, rather than leaking a partial vector.
