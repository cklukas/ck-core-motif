---
nav_exclude: true
---

# TeachingTip visibility API inputs

The public TeachingTip show, hide, and visibility APIs now reject a null
widget before invoking the Xt subclass check. Invalid optional handles return
the existing no-op or false result; valid delay, popup, positioning, and
dismissal callback behavior is unchanged.

The focused regression check is
`tests/automated/check-teaching-tip-inputs.py`.
