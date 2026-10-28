---
nav_exclude: true
---
# IconFile directory-cache offset bounds

The valid-directory icon cache stores name offsets as `unsigned short`. Cache
construction now rejects a directory whose combined icon-name heap exceeds
`USHRT_MAX`, before allocating or writing the offset table. Such directories
use the existing uncached fallback rather than silently truncating offsets and
returning the wrong icon entry.
