---
nav_exclude: true
---

# MWM command-tree lifecycle

Command-branch removal advances its owned node pointer for every sibling and
releases each node's name and default label exactly once. Command deletion uses
pointer-to-pointer traversal so deleting a head entry continues through the
remaining siblings and duplicate IDs, including nested branches.

The previous branch loop retained the first node pointer, freeing it repeatedly
when a command cascade had multiple children. Replacing Wsm's command menus
could consequently crash MWM. Selection-owner recovery also removes commands
belonging to the departed workspace manager on the correct managed screen.

`tests/automated/run-mwm-command-tree-lifecycle.py` runs the production deletion
functions under ASan/UBSan with head, sibling, nested and repeated-ID cases and
checks release of all node/name/label allocations. The native Wsm acceptance
suite exercises replacement sessions, menu regeneration and MWM restart.
