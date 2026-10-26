# DragBS atom-entry growth

When a Motif atom table is exhausted, DragBS appends a new entry and rewrites
the property.  The append now checks the Xt byte range, uses a temporary
reallocation pointer, and increments the entry count only after success.  A
failed resize releases the server grab and leaves the table consistent.
