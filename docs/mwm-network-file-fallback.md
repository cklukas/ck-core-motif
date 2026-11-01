---
nav_exclude: true
---
# MWM network-file fallback

When ToolTalk resource-path expansion cannot tokenize a `host:file` name,
MWM falls back to a private writable copy before separating the host and file
at the colon.  The fallback now passes that separated owned host to ToolTalk,
rather than the original unsplit input.  Network-file canonicalization can
therefore still resolve the intended host and file after shell expansion
fails.

The `~/` expansion test first establishes the two ASCII prefix bytes before
asking the multibyte decoder to inspect them. Empty and one-character malformed
paths therefore remain ordinary fallback inputs instead of causing a decoder
probe past the string terminator.

`tests/automated/check-mwm-network-file-fallback.py` protects the split-string
ownership contract.
