---
nav_exclude: true
---
# PushButton emphasis fallback

PushButton and PushButtonGadget shadow/highlight routines now tolerate an
invalid `default_button_emphasis` value by returning without drawing. The
documented external and internal emphasis modes retain their existing behavior;
only malformed values avoid terminating the application.

The regression check is
`tests/automated/check-pushbutton-emphasis-fallback.py`.
