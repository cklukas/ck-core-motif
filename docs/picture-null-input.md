# Picture parser null input

`XmParsePicture` now returns `NULL` when given no specification, before it
attempts to copy or parse the input string.  This makes malformed caller input
fail safely.
