# FileSB empty text fallbacks

File Selection Box filter and directory text creation now use a static empty
string when their one-byte fallback allocation fails.  The widget still gets
created, and the cleanup path only frees buffers owned by the allocator.

The regression check is `tests/automated/check-filesb-empty-text.py`.
