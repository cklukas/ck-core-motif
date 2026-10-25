---
nav_exclude: true
---
# MWM match-list insertion allocation

Client-command matches can insert new menu items.  The insertion path now
passes source strings to the constructor, which owns the copies, avoiding the
previous temporary-string leaks.  It also skips an insertion cleanly when
item construction fails instead of dereferencing a null result.  Successful
insertions retain their labels, arguments, ordering, and command IDs.

`tests/automated/check-mwm-match-insertion-allocation.py` protects this
contract.
