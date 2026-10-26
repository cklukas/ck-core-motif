---
nav_exclude: true
---
# Xmos qualified-path allocation guards

`GetQualifiedDir` now checks each path-buffer allocation before writing the
home-directory, absolute, relative, or fallback path. Allocation failure
returns NULL, matching the function’s existing failure result without a
partial or corrupted pathname.

`_XmOSInitPath` also bounds the copy of an environment-provided resource path
before converting its `size_t` length to Xt's `Cardinal` allocation size. An
unrepresentable value now fails cleanly instead of truncating the allocation.

`XmeGetHomeDirName` applies the same bound to `$HOME` and passwd-derived home
directories, returning the existing empty-name fallback when the length cannot
be represented by Xt.

The focused regression is
`tests/automated/check-xmos-qualified-path-allocation.py`.
