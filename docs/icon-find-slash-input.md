# IconFile path scanner input guard

`find_slash` now returns immediately for a null path before entering either
single-byte or multibyte scanning logic. This keeps internal icon lookup paths
safe regardless of the active locale and preserves behavior for valid strings.

The focused regression is `tests/automated/check-icon-find-slash-input.py`.
