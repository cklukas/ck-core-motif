---
nav_exclude: true
---
# XPM next-string lookahead initialization

When scanning buffer-backed XPM data with natural comment markers, the parser
can encounter the end of the buffer before entering its lookahead loop. The
lookahead character is now initialized to `NUL`, so the post-loop check is
defined and does not consume an indeterminate value. Normal non-empty scans
retain their existing behavior.

The focused regression is `tests/automated/check-xpm-next-string-init.py`.
