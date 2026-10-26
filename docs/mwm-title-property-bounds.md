# MWM title-property bounds

WM_NAME and WM_ICON_NAME arrive as `XTextProperty` records with an explicit
item count.  MWM now uses `nitems` to reject empty values instead of calling
`strlen` on the raw Xlib buffer, which is not required to be NUL-terminated.
Non-empty title and icon-name properties continue through the existing Motif
conversion path unchanged.
