# MWM icon-box graphics-context masks

The icon-box fade and shrink-wrap graphics contexts copy every GC field from
the inactive icon GC. Their all-bits copy mask is now explicitly expressed in
Xt's unsigned `XtGCMask` type, rather than relying on a signed `long` value
being implicitly converted at the Xlib boundary.

The focused source contract is covered by
`tests/automated/check-mwm-icon-gc-mask.py`.
