---
nav_exclude: true
---

# XmIm missing-shell-data update guard

`XmIm` resource updates can race widget teardown: the display-level XIM may
still be available after the shell's private `XmImShellInfo` has been freed.
The update routine now returns safely when that per-shell record is absent.

Normal realized-shell updates are unchanged, and no public ABI or resource
semantics change.  The static regression check is
`tests/automated/check-xmim-missing-shell-data.py`.
