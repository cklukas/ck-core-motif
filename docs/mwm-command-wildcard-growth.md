# Unbounded client-command wildcard matching

MWM client-command matching no longer stores wildcard segment positions in a
fixed 100-entry stack array.  Menu specifications and command-match exclusions
with more wildcard segments now grow their bookkeeping dynamically, with the
same `Cardinal` allocation bounds used by the surrounding Xt code.

If bookkeeping allocation cannot be extended, the matcher keeps its existing
safe fallback: it retains the prior best match rather than using incomplete
comparison state.  Normal matching rules remain unchanged: fewer wildcards
win, followed by wildcards appearing farther to the right.

`tests/automated/check-mwm-command-wildcard-growth.py` checks that both sides
of the comparison use the checked dynamic storage and release it on every
return path.
