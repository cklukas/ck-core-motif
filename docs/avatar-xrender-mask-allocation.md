# Avatar XRender mask allocation bounds

Modern Avatar rendering constructs an A8 XRender coverage mask only for a
positive, bounded diameter.  After Xlib creates the temporary `XImage`, Avatar
also verifies that its row stride is positive and that the requested rows fit
the allocation domain before assigning image data.

An unsupported image layout now falls back to the existing non-XRender drawing
path rather than allocating through a signed or overflowing stride.  The
normal circular image and initials rendering path is unchanged.

Regression: `tests/automated/check-avatar-pixmap-allocation.py`.
