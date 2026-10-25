---
nav_exclude: true
---
# Protocol manager allocation

Protocol registration now treats manager creation and manager-list growth as
transactional.  A failed manager allocation or list resize leaves the
existing per-shell protocol state unchanged, and callers stop cleanly instead
of dereferencing a null manager.  The initial per-shell manager allocation is
also checked before installing callbacks or saving the X context.

Valid protocol registration and callback behavior are unchanged.  The focused
source-level regression is
`tests/automated/check-protocol-manager-allocation.py`.
