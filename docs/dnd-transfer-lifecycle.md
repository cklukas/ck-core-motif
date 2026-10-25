# Drag-and-drop transfer qualification

`tests/automated/check-dnd-transfer.c` is an installed-prefix consumer probe
for the public Motif drag-and-drop transfer path. It creates a source and a
destination Label in separate shells on one X display, registers the
destination as a drop site, starts a real Motif drag through XTEST pointer
events, and completes one `XmDropTransferStart` request.

The probe checks the following boundaries:

- the drop callback receives a valid drag context, valid-site status and the
  requested copy operation;
- the source conversion callback uses the full Xt selection-conversion
  signature and returns the advertised target, bytes and format;
- `XmDropTransferStart` preserves the transfer entry's `client_data` through
  the asynchronous selection callback; and
- the selection value remains owned by Xt after the callback, so the consumer
  does not free it.

Run it against a fresh installed prefix with:

```sh
python3 tests/automated/run-dnd-transfer.py \
  /absolute/installed/prefix /absolute/fresh/results
```

The AArch64 `uconsole` qualification on 2026-10-25 passed 11 checks in three
fresh Xvfb runs. All three receipts used the same `libXm.so.8.0.0` SHA-256
(`2c0e75e58ec0c0e2a0cd6585f06ee4a695f54f79633b307ad47405ba0a703fa3`) and
the same probe source SHA-256
(`11d44ec7220c54721a5d68979c6126ee4b8fbda1e9a6c8fd99ca9e8c063992d3`). The
runner records the exact prefix, compiler output, loaded-library proof,
source/library hashes, output path and elapsed time.

This is a bounded same-process transfer contract. It does not claim
cross-process transfer, cancellation, owner replacement, incremental payload
assembly, multiple targets, or a complete application drag-and-drop journey;
those remain separate qualification gates.

`tests/automated/check-dnd-cancel.c` covers the related initiator-side
cancellation contract. It starts a Motif drag, calls the public
`XmDragCancel` API while the drag is active, and verifies the
`XmNdragDropFinishCallback` completion notification. Three fresh AArch64
`uconsole` Xvfb runs on 2026-10-25 passed three checks each, using the same
`libXm.so.8.0.0` hash shown above; the cancellation runner records the same
receipt and loaded-library evidence.

This does not promote selection-transfer cancellation, cross-process owner
replacement, incremental transfer cancellation or application-level journeys
to complete.
