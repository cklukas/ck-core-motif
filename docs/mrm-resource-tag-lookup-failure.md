# Mrm resource-tag lookup failures

When an argument tag uses a compressed resource code, a failed
`Urm__UncompressCode` lookup now stops processing that argument immediately
after reporting the error. The invalid lookup result is no longer passed to
string comparison or later resource conversion, while valid tags retain their
existing behavior.

The source-level regression check is
`tests/automated/check-mrm-resource-tag-lookup-failure.py`.
