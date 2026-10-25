# XmString unoptimized-entry copy bounds

Unoptimized XmString entries own optional text and rendition begin/end tag
arrays.  The copy path previously copied the source pointers into the new
entry and then allocated replacement arrays without checking the count or
allocation result.  Failure could expose source ownership to cleanup or
leave a null array for the subsequent copy loop.

The new entry clears its owned array pointers before allocation, checks each
count in `size_t`, and releases the partially built entry on failure.  Normal
copy ownership and representation remain unchanged.
