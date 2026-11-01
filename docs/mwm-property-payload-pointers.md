# MWM X11 property payload validation

Several MWM property readers receive an allocated payload pointer from Xlib.
The readers now require that pointer to be non-null alongside the expected
type, format, and item count before copying, parsing, or returning data.  This
keeps malformed or failed property replies from becoming null-pointer
accesses, including the WM_STATE and _MWM_HINTS getters.
The same contract now covers the WM_PROTOCOLS, _MWM_MESSAGES, and
WM_COLORMAP_WINDOWS arrays before their elements are consumed.

`WM_PROTOCOLS` additionally treats a zero-item property as absent.  It no
longer asks Xt for a zero-byte protocol array or emits a misleading allocation
warning for an empty client property.
