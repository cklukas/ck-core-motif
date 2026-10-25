---
nav_exclude: true
---
# Text line look-pointer guard

Text line selection uses `Look()` to obtain a character pointer. The end-of-
line guard now checks that pointer against `NULL` before dereferencing it,
instead of comparing the pointer itself with the character constant `NUL`.
This preserves normal line selection and makes end-of-buffer handling defined.

The focused regression is `tests/automated/check-text-line-look-pointer.py`.
