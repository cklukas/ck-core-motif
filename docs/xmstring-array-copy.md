# XmString array-entry copy bounds

Copying an array entry allocates a pointer table for all child segments and
then recursively copies each child.  The old path multiplied the segment
count without a `size_t` bound and assumed every recursive copy succeeded.

The copy now checks the table byte count, handles allocation failure, and
frees already-copied children before returning.  Normal copies retain the
same entry layout and ownership behavior.
