# Mrm resource reload bounds

`Idb__INX_FindResources` now revalidates the current node entry’s string after
buffer turnover reloads the node record.  The traversal therefore checks the
actual rebound record before appending its name, preserving safe behavior when
serialized index data is malformed.

The source-level regression is
`tests/automated/check-mrm-resource-reload-bounds.py`.
