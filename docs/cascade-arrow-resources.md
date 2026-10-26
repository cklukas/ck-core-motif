---
nav_exclude: true
---
# Cascade-arrow resources

Cascade buttons cache armed and unarmed arrow pixmaps.  The cache records,
temporary graphics contexts, and server pixmaps are now checked before use.
Pixmap creation failures leave the cache entry unavailable rather than issuing
draw requests against `None`; temporary GC ownership is also released on all
failure and success paths.
