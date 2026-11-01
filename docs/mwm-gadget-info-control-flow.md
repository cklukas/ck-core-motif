---
nav_exclude: true
---
# MWM gadget-info control flow

Mwm automation replies describe title-bar and resize gadgets. The fallback
paths that fill unavailable gadget records are now explicitly braced in the
maximize, move, and resize reply builders. This makes the fallback scope
unambiguous to maintainers and strict compilers while leaving the packed reply
layout and valid-gadget behavior unchanged.

Move and window-menu reply builders additionally initialize their temporary
records to the same invalid sentinel that `FillInvalidInfo` emits. A reply
therefore cannot expose uninitialized geometry even if a later maintenance
change bypasses a fallback branch.

`tests/automated/check-wmcmd-control-flow.py` protects these control-flow
boundaries. The focused Mwm compilation uses
`-Werror=misleading-indentation` against the real source.
