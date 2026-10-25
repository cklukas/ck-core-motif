# XmString entry creation allocation

The internal `_XmEntryCreate` macro now checks each `XtMalloc` result before
zeroing or initializing the record.  Allocation failure therefore produces a
null entry that callers can unwind, instead of dereferencing a null pointer.
Successful entry initialization is unchanged.
