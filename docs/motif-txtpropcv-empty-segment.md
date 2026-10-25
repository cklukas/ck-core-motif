# Empty text segment separators

`GetTextSegment` can receive an `XmNO_TEXT` segment with tabs or a separator
attached.  Separator insertion now treats a null segment buffer as an empty
string, preserving the existing tab/newline formatting without calling
`strlen` or `strcpy` on null storage.

The regression check is
`tests/automated/check-txtpropcv-empty-segment.py`.
