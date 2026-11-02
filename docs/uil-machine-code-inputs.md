---
nav_exclude: true
---
# UIL machine-code input and listing growth

UIL now rejects a machine-code record with a missing nonempty byte buffer and
does not append beyond the 16-bit source-record item count.  This avoids a
null copy and prevents count wrap from making the record list disagree with
its stored count.

When the listing renderer needs a larger reverse-order vector, it now retains
the existing allocation until replacement allocation succeeds.  An out-of-memory
listing therefore stops safely rather than dereferencing a null vector.  Word
splitting also uses the actual `unsigned long` element size.

Regression coverage: `tests/automated/check-uil-machine-code-inputs.py` and
`tests/automated/run-uil-machine-code-inputs-unit.py`.
