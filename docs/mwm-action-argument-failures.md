---
nav_exclude: true
---

# MWM action-argument parser failures

MWM parses `f.action` arguments into an owned action descriptor.  It does not
allocate an argument array until it reads the first argument, so a valid
zero-argument action has no needless allocation requirement.  A failed
argument allocation, an unrepresentable argument length, an unavailable
environment-variable expansion, or failed argument-array growth must not leave
the descriptor usable with only the arguments parsed so far.  The parser
releases that partial descriptor, clears the returned argument pointer, and
reports conversion failure.  This prevents an action from running with a
silently shortened argument list.

Regression: `tests/automated/check-mwm-action-argument-failures.py`.
