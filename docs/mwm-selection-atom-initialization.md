# MWM per-screen selection atom initialization

`mwm` maintains one `WM_Sn` selection atom for each physical X11 screen.
Only screens managed by the current window-manager instance acquire a named
selection.  The backing table is now zero-initialized, so every unmanaged
entry is explicitly `None` rather than indeterminate storage.

This preserves the existing public selection names and ownership behavior.
It makes callback-side comparisons deterministic if a screen is unmanaged or
if setup cannot install a selection callback for that screen.

`tests/automated/check-mwm-selection-atom-initialization.py` verifies the
allocation contract without requiring a nested window-manager session.
