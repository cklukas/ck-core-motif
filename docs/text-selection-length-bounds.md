# Text selection length bounds

Selection and drop imports now validate the length of converted locale text
before storing it in Motif's legacy `int` text-block field. Oversized converted
strings are rejected with their temporary buffers released, preventing a
wrapped length from reaching text replacement or follow-up move/delete logic.

The focused source contract is checked by
`tests/automated/check-text-selection-length-bounds.py`.
