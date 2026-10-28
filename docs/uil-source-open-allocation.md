# UIL source-opening allocation failures

`src_open_file` allocates a file-control block and, when no reusable buffer is
available, a source buffer.  Both objects are required by the subsequent open
and source-table setup.  The function now checks each allocation before use,
releases the file-control block if the second allocation fails, and reports
the failure through the existing Xt error mechanism.
