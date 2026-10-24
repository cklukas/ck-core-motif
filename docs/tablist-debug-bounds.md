# XmTabList debug formatting bounds

The optional XmTabList debug dump renders unknown unit, offset-model, and
alignment enum values into static diagnostic buffers.  Those values are
normally small, but malformed or future values should not be able to corrupt
the debug process.

All three descriptions now use `snprintf` with the destination size.  Known
enum text and the debug output format are unchanged.

`tests/automated/check-tablist-debug-bounds.py` protects these paths.
