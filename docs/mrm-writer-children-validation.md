# Mrm writer children validation

`UrmCWRInitChildren` now propagates invalid-context failures before sizing or
allocating the child descriptor. Valid child counts retain the existing size
limits and UID serialization behavior.

The source-level regression check is
`tests/automated/check-mrm-writer-children-validation.py`.
