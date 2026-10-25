# XmStringPutRendition allocation failures

`XmStringPutRendition` now treats failure to create either rendition-boundary
component as an operation failure.  It no longer returns a partially modified
or unadorned copy, and releases the intermediate string when the closing
component cannot be allocated.
