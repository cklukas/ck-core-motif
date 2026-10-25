---
nav_exclude: true
---
# XmString context rendition growth

Active-rendition growth is now transactional.  The context checks count and
`Cardinal` size arithmetic, retains the existing tag array when `XtRealloc`
fails, and marks the context error instead of copying through a null or
wrapped buffer.
