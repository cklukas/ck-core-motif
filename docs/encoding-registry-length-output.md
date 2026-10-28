# Encoding-registry length output

`_XmGetEncodingRegistryTarget` now returns `NULL` when its output-length
pointer is absent instead of dereferencing it. Existing callers that provide a
length pointer retain the same registry serialization and allocation behavior.

Regression coverage: `tests/automated/check-encoding-registry-length-output.py`.
