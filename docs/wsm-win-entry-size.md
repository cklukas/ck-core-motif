---
nav_exclude: true
---

# WSM window-entry reply sizing

`WinEntrySizePacked` accounts for a window-entry reply's window IDs and
match-property IDs.  The match-property count field must be added after the
window-ID array; resetting the running size there underallocated replies that
contained both arrays.  The calculation now retains both contributions, while
the protocol layout remains unchanged.
