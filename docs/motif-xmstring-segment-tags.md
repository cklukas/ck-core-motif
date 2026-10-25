---
nav_exclude: true
---
# XmString rendition-tag validation

Rendition-tag copying now rejects a null tag table or null entry before calling
`XtNewString`. Previously malformed input could reach `strlen` through the
allocator and crash. Cleanup remains transactional, and borrowed tag pointers
are never freed when the caller requested a non-copying operation.
