---
nav_exclude: true
---
# Mrm writer arg-value rebinds

Each type-specific branch in `UrmCWRSetArgValue` now propagates the
argument-descriptor rebind result after record growth.  A failed rebind stops
before writing the serialized offset, while successful type conversions retain
their existing behavior.

The source-level regression is
`tests/automated/check-mrm-writer-arg-value-rebind.py`.
