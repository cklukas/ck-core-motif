# Image-cache warning state

Several image-cache callbacks receive hash keys or closure pointers required by
the generic callback ABI but not by their particular operation. The lock
wrappers also intentionally compile to no-ops in non-threaded builds. These
paths now mark that state explicitly, keeping strict warning builds useful
without changing callback signatures or locking behavior.

The source contract is covered by
`tests/automated/check-image-cache-warning-state.py`.
