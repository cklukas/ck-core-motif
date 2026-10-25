---
nav_exclude: true
---
# Xmos directory-cache entry bounds

`AddEntryToCache` stores each directory name in the flexible trailing
`file_name` member of `XmDirCacheRec`. The allocation now includes one byte
for the terminating NUL written by `strcpy`. This preserves the existing
directory-cache behavior while preventing a one-byte heap overwrite for every
cached entry.

`tests/automated/check-xmos-cache-bounds.py` guards the allocation contract.
