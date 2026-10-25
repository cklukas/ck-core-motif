# Label accelerator allocation

Menu labels own a private copy of their accelerator string.  `SetValues` now
publishes a replacement only after `XtNewString` succeeds; if allocation fails,
the currently installed accelerator remains active and the requested binding
is discarded without freeing the valid old copy.  The `GetValues` hook also
returns `NULL` directly when a label has no accelerator instead of passing a
null pointer to `XtNewString`.

Successful accelerator replacement and the existing non-menu behavior are
unchanged.  This keeps menu bindings usable during allocation pressure and
makes querying an absent accelerator safe.
