# XmString context-copy allocation

`_XmStringContextCopy` checks both the size calculation and allocation of the
active rendition-tag array. Counts are promoted before multiplication and the
byte count is bounded by the `Cardinal` allocator interface. The destination
clears its copied pointer before allocation, so an unrepresentable or failed
copy leaves an empty active-rendition set without borrowing the source array.
