# Clipboard end-copy record validation

`XmClipboardEndCopy` now validates the finalized data-item retrieval status,
pointer, and record length before reading cut-by-name fields. Invalid records
are released and follow the existing corrupt-clipboard handling instead of
being dereferenced as a shorter or missing structure.
