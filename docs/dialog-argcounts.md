---
nav_exclude: true
---
# Dialog argument counts

Command and MessageBox convenience constructors add one internal argument to
the caller's list. They now reject the maximum `Cardinal` count before that
addition and allocation can wrap. Normal argument lists and dialog resources
are unchanged.
