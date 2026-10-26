---
nav_exclude: true
---
# XPM pixmap resource creation

The XPM image conversion helper now validates its input image and output
pointer, checks `XCreatePixmap`, and checks `XCreateGC` before calling
`XPutImage`.  If GC creation fails, the pixmap is released and the result is
reset to `None`; callers therefore receive a clean failure rather than an
invalid server request.
