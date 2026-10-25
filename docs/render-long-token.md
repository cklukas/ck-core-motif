---
nav_exclude: true
---
# Long render-table tokens

Alphabetic render-table tokens are now allocated to their actual length.  Long
font or tag names are preserved as one token instead of being truncated at 79
bytes and misparsed as multiple tokens.
