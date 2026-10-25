---
nav_exclude: true
---
# FileSB qualified-mask allocation failure

File Selection Box search qualification builds a combined directory and
pattern mask before invoking the directory and file search callbacks.  The
temporary mask allocation is now checked before it is populated.  On failure,
all intermediate text buffers are released and the output callback record is
initialized with null strings, allowing the normal search path to reject the
request safely.

The regression check is
`tests/automated/check-filesb-qualified-mask-allocation.py`.
