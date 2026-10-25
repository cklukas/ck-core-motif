# FileSB conversion target allocation

When a File Selection Box advertises file and file-name conversion targets,
the temporary atom array is now checked before initialization.  Allocation
failure refuses the conversion cleanly instead of dereferencing null storage.

The regression check is
`tests/automated/check-filesb-convert-allocation.py`.
