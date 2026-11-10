# MWM client-metadata refresh inputs

The MWM client-metadata refresh entry points accept a `ClientData *` supplied
by event and session-management paths.  They now treat a null record as a
no-op before accessing the display, client window, cached strings, hints, or
title state.

This applies to class, session-client ID, save-hint, WM-hint, normal-hint,
window-title, icon-title, and Motif-hint processing.  A valid client record
continues through the established property and resource processing unchanged.

The focused check is:

```
python3 tests/automated/check-mwm-client-metadata-inputs.py
```

It verifies that every covered entry point has its null guard before its first
client-record member access.
