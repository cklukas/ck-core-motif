# WmFeedback size-limit header

`clients/mwm/WmFeedback.c` compares feedback-string lengths with `UINT_MAX`
before allocating a copy.  The source now includes `<limits.h>` directly so
the guard is defined when Mwm is compiled independently of incidental
transitive headers.  This preserves the existing allocation behavior while
making the Mwm build portable across C library/header layouts.

The automated check is `tests/automated/check-wm-feedback-size-limit-header.py`.
