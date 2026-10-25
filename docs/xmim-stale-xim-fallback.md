---
nav_exclude: true
---

# XmIm stale-XIM fallback guard

An input method can be closed and reopened while an existing input context is
still referenced.  The unrecognized-resource fallback previously asserted
that the XIC belonged to the cached XIM, which could abort during this normal
lifecycle transition.

The fallback now returns without recreating the XIC when the ownership differs.
The existing context remains intact, valid same-XIM updates are unchanged, and
the public Motif ABI is unchanged.  The static regression check is
`tests/automated/check-xmim-stale-xim-fallback.py`.
