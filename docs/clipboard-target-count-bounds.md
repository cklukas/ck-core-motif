---
nav_exclude: true
---
# Clipboard target-list count bounds

Clipboard target lists arrive through converter callbacks with an unsigned
length. The source path now rejects lengths that cannot be represented by its
legacy `int` loop counter before narrowing, and closes the active clipboard
copy cleanly on that failure.

The focused regression is
`tests/automated/check-clipboard-target-count-bounds.py`.
