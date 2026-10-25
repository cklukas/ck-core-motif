---
nav_exclude: true
---
# Container detail-header cache guard

When an icon header changes, Container now clears its old cached detail-header
table before allocating the replacement. Allocation failure leaves the cache
empty and returns safely instead of retaining a dangling pointer and writing
through it.
