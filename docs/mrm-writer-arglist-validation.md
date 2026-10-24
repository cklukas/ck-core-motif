# Mrm writer arglist validation

`UrmCWRInitArglist` now propagates invalid-context failures before sizing or
allocating an argument descriptor.  Valid contexts retain the existing
argument-count limits and UID serialization behavior.

The source-level regression is
`tests/automated/check-mrm-writer-arglist-validation.py`.
