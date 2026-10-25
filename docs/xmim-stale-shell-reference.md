---
nav_exclude: true
---

# XmIm stale shell-reference guard

Input-method shell references and vendor-shell extension data are released by
different teardown paths.  During that window, reconstructing an application-
created XIC can encounter a reference whose `XmImShellInfo` is already gone.

The reconstruction walk now skips that stale entry and continues searching.
Live shell records retain the same lookup behavior, and the public Motif ABI is
unchanged.  The static regression check is
`tests/automated/check-xmim-stale-shell-reference.py`.
