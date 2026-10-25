---
nav_exclude: true
---

# Compound-text control-sequence bounds

Malformed compound text can end an ESC or CSI sequence before its second byte.
The dispatchers now reject those truncated sequences before indexing the
sequence buffer.

Valid control sequences and the public Motif ABI are unchanged.  The static
regression check is `tests/automated/check-compound-text-control-sequence-bounds.py`.
