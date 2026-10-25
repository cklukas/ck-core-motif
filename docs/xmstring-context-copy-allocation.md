# XmString context-copy allocation

`_XmStringContextCopy` now checks allocation of the active rendition-tag
array.  If the copy cannot allocate that optional array, the copied context
keeps its string and scalar state but exposes an empty active-rendition set
instead of dereferencing a null pointer.
