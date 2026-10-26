# MWM binding replacement token boundaries

`FixupBindingsString` replaces vendor key names in the root-window virtual-key
bindings property. The original-name length is kept in `size_t` so a long
property cannot narrow the length before suffix and allocation arithmetic are
checked.

A replacement at the first byte of the property is a valid token. The
boundary check therefore accepts `ks_ptr == fixed_str` without evaluating
`ks_ptr - 1`; forming that pointer would refer before the allocated string and
is undefined behavior. Replacements elsewhere still require a preceding
space, tab, or `>` and a following whitespace boundary, preserving the
existing token-matching rules.

The property writer also rejects strings longer than Xlib's signed `int`
item-count parameter instead of allowing an implicit narrowing conversion.
Allocation failure while copying the input property returns `NULL` before the
replacement loop starts.

The source contract is checked by
`tests/automated/check-mwm-binding-string-growth.py`.
