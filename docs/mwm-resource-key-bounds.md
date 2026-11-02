# MWM resource-key construction bounds

MWM builds several X resource keys from a screen name and up to three
component names.  Those keys share the legacy `wmGD.tmpBuffer` buffer.

The builder now tracks the remaining buffer space as an unsigned `size_t` and
uses one bounded append helper for every name and separator.  If a component
fills the buffer, later components are ignored.  In particular, a truncated
third component can no longer make the old signed remaining-count negative and
turn the fourth append into an unbounded `strncat` call.

Ordinary keys retain their existing `*`-separated form.  The focused source
regression is `tests/automated/check-mwm-resource-key-bounds.py`.
