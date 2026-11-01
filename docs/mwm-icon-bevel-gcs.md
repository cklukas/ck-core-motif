# MWM icon bevel graphics contexts

When MWM composes a framed custom icon, it obtains temporary top- and
bottom-shadow GCs with `XtGetGC`.  The temporary references are now released
after drawing and if only one acquisition succeeds.  This preserves the
rendering while preventing repeated icon creation from accumulating Xt GC
references.

The focused source contract is covered by
`tests/automated/check-mwm-icon-bevel-gc-release.py`.
