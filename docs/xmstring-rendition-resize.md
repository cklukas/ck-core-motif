# XmString rendition resize failures

Rendition begin/end setters now resize their tag arrays transactionally. The
existing pointer and count remain unchanged when `XtRealloc` fails, avoiding a
null dereference and preserving the entry's prior formatting state.
