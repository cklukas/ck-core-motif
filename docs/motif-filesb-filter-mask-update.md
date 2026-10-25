# FileSB filter-mask update allocation failure

When a File Selection Box refreshes its full-path filter field, it assembles a
temporary directory-and-pattern string.  The temporary buffer is now checked
before it is populated or passed to the text field.  If allocation fails, the
existing filter text remains unchanged and the source strings are still
released.

The regression check is
`tests/automated/check-filesb-filter-mask-update.py`.
