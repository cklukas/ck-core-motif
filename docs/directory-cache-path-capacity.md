---
nav_exclude: true
---
# Directory-cache full-path capacity

The file-selection directory cache constructs a temporary full path to classify
each directory entry with `stat`. Its buffer now tracks its allocation and grows
with a checked Xt allocation when an entry name exceeds the former fixed
user-name-sized slack. Long valid directory-entry names are classified without
writing beyond the cache path buffer; ordinary cached-path behavior is
unchanged.

The focused regression check is
`tests/automated/check-directory-cache-path-capacity.py`.
