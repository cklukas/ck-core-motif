# Virtual-key binding-file allocation

`_XmVirtKeysLoadFileBindings` builds a NUL-terminated binding string from
possibly long lines. Growth and final trimming now use temporary pointers and
check allocation results; invalid input pointers are rejected before file
access.

Failure closes the file and returns `False` without publishing a null or
partially resized buffer. Successful binding text remains unchanged.
