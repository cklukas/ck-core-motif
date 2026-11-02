# File Manager unreadable-entry fallback

The File Manager keeps an entry visible when its metadata lookup fails.  That
fallback now avoids indexing the file-info array with the `-1` default-icon
sentinel or reading uninitialized `stat` fields.  It displays placeholder
owner information, uses non-directory defaults, and builds the metadata path
and icon-mask names with bounded storage.

The focused source-contract check is:

```sh
python3 tests/automated/check-filemanager-unreadable-entry.py
```
