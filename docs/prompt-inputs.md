---
nav_exclude: true
---

# Prompt visibility API inputs

The public Prompt visibility setter and getter now reject a null widget before
invoking the Xt subclass check. Invalid optional handles return the existing
no-op or false result; valid visibility and redraw behavior is unchanged.

The focused regression check is `tests/automated/check-prompt-inputs.py`.
