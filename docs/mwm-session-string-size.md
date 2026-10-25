---
nav_exclude: true
---

# MWM session workspace and host string sizing

Session restore records copy workspace lists and client host names into Xt
storage. Both fields now use one checked allocator that rejects lengths which
cannot be represented by Xt's allocation type before copying. Existing valid
strings and the session record format are unchanged.

The focused regression check is
`tests/automated/check-mwm-session-string-size.py`.
