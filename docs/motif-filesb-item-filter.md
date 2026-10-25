# FileSB item filter allocation failure

When a File Selection Box restores its full-path filter field, it combines the
current directory and pattern in a temporary buffer.  The buffer is now
checked before it is populated or passed to the text field.  If allocation
fails, the existing filter text remains unchanged and the source strings are
still released.

The regression check is
`tests/automated/check-filesb-item-filter-allocation.py`.
