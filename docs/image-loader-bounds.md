---
nav_exclude: true
---
# JPEG and PNG image allocation bounds

The JPEG and PNG loaders validate decoded dimensions and row sizes before
forming pixel-buffer allocations.  This prevents width×height or rowbytes×height
wraparound from producing undersized buffers, and makes allocation failure a
normal image-load error instead of a later null-buffer dereference.  The
public `XmGetPixmap` image-loading behavior and decoded image formats remain
unchanged for valid images.

The native image regression suite covers malformed image handling and image
scaling after rebuilding the installed library.
