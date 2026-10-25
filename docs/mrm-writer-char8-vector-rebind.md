# Mrm writer Char8-vector rebind failures

`UrmCWRSetArgChar8Vec` now checks the argument descriptor rebind immediately
after allocating the vector, as well as after each appended string. Invalid
argument-list state therefore fails before the vector header is dereferenced.

The source-level regression check is
`tests/automated/check-mrm-writer-char8-vector-rebind.py`.
