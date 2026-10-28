---
nav_exclude: true
---

# Drop-transfer payload validation

`DropTransferSelectionCB` now fails a transfer before invoking application
code when Xt reports a nonzero payload length with a `NULL` value pointer.
It also rejects nonempty payloads with a format other than X11's 8, 16, or
32-bit formats, or with no target type atom.
Zero-length callbacks remain valid, including incremental-transfer completion;
well-formed payloads retain their existing callback and progression behavior.

The focused source-contract check is
`tests/automated/check-drop-transfer-callback-selection.py`.
