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

This is a bounded same-process single-target transfer contract. Multiple
targets, cancellation and owner replacement are qualified by separate probes;
incremental payload assembly and a complete application drag-and-drop journey
remain separate gates.

`tests/automated/check-dnd-cross-process.c` is the companion two-client probe.
It forks before opening displays, gives the source and destination independent
Xt application contexts and X11 selection owners, and coordinates only through
a private Unix socket. The source's XTEST drag crosses from the fixed
source-client window at `(20,20)` to the destination-client window at
`(320,20)`, so the fixture never dereferences a widget owned by the other
process. It checks the same conversion, drop-admission, transfer-entry and
Xt-owned-value boundaries across the X11 client boundary.

Run it with:

```sh
python3 tests/automated/run-dnd-cross-process.py \
  /absolute/installed/prefix /absolute/fresh/results
```

The AArch64 `uconsole` qualification on 2026-10-25 now runs three modes. The
single-target mode passed 14 checks, the multiple-target mode passed 22 checks,
and the incremental mode passed 116 checks in three fresh Xvfb runs each. The
incremental mode requests a 4 MiB payload and verifies every non-empty Xt
chunk, its byte total and the terminating callback. All receipts used the
installed `libXm.so.8.0.0` SHA-256
(`1a38e360f82964e7a318bc61099f7300be6c05ec5a75c3e09814646f9adf545`) and the
probe source SHA-256
(`84f11e5c0660ef5bab9db67a4f4b9b2deab2daef8aa132887b0e6fa89690d9b4`).
This qualifies one independent-client copy transfer, its two-target array
contract and incremental payload assembly; a complete application DnD
workflow remains a separate gate.

`tests/automated/check-dnd-transfer-cancel.c` covers the receiver-side
DropTransfer lifetime contract. Its immediate mode destroys the object before
the delayed start timer runs; its active mode requests a 2 MiB incremental
selection and destroys the object from the first transfer callback; and its
requestor mode destroys the destination shell while the request is queued.
The toolkit now retains and removes the delayed timer, unregisters the
drag-context and requestor callbacks, cancels an outstanding Xt selection
request when destruction really interrupts it, and only forgets a request that
has already completed. Completed transfer arrays are nulled after release so
the later source-notification destruction cannot free them twice.

Run it with:

```sh
python3 tests/automated/run-dnd-transfer-cancel.py \
  /absolute/installed/prefix /absolute/fresh/results
```

The latest AArch64 `uconsole` qualification on 2026-10-25 passed the immediate
7-check, active 8-check and requestor 7-check modes in three fresh Xvfb runs
each. All receipts used the installed `libXm.so.8.0.0` SHA-256
(`1a38e360f82964e7a318bc61099f7300be6c05ec5a75c3e09814646f9adf545`) and
the probe source SHA-256
(`f872d518414a7b8cfe36a5eb693669bedffadcc867c9c285788fb8089866df9a`).
This closes the bounded receiver-object/timer/requestor cancellation contract;
source-side cancellation and owner replacement are covered separately below;
multiple targets and complete application workflows remain separate gates.

`tests/automated/check-dnd-cancel.c` covers the related initiator-side
cancellation contract. It starts a Motif drag, calls the public
`XmDragCancel` API while the drag is active, and verifies the
`XmNdragDropFinishCallback` completion notification. Three fresh AArch64
`uconsole` Xvfb runs on 2026-10-25 passed three checks each, using the same
`libXm.so.8.0.0` hash shown above; the cancellation runner records the same
receipt and loaded-library evidence.

This does not promote source-side cancellation after a drop, owner
replacement, multiple-target transfer or application-level journeys to
complete.

`tests/automated/check-dnd-source-cancel.c` covers source-side selection loss.
The owner-replacement mode takes the source selection while the pointer drag
is still active; the post-drop mode starts an incremental 4 MiB transfer and
replaces the source owner while the receiver is requesting data.  The source
now treats selection loss as a terminal ownership event: an active pointer
drag is cancelled, while a pointer-complete but not-yet-finished drop is
completed with failure.  Both paths retire the drag context exactly once and
retain the existing warning so an unexpected owner replacement remains
diagnosable.

Run it with:

```sh
python3 tests/automated/run-dnd-source-cancel.py \
  /absolute/installed/prefix /absolute/fresh/results
```

The AArch64 `uconsole` qualification on 2026-10-25 passed the owner-
replacement mode with 8 checks and the post-drop mode with 14 checks in three
fresh Xvfb runs each.  The installed `libXm.so.8.0.0` hash was
`1a38e360f82964e7a318bc61099f7300be6c05ec5a75c3e09814646f9adf545a`; the
source probe hash was
`275a0576d79c1e971553a7b723e82f94f65d0942fe986b182011ad94e0aa5ec6`.
The same prefix also passed the existing same-process (11 checks),
cross-process (12 checks) and initiator-cancellation (3 checks) probes in
three fresh runs each.  Cross-process multiple-target and full application
DND workflows remain open.

`tests/automated/check-dnd-multiple-targets.c` covers a two-target transfer in
one `XmDropTransferStart` request.  The source exports two atoms with distinct
payloads, the destination imports both, and the transfer callback verifies
that each target retains its own client data, type, format and bytes exactly
once.  This exercises the array ownership and completion path without
introducing a new public API.

Run it with:

```sh
python3 tests/automated/run-dnd-multiple-targets.py \
  /absolute/installed/prefix /absolute/fresh/results
```

The AArch64 `uconsole` qualification on 2026-10-25 passed 21 checks in three
fresh Xvfb runs.  The installed `libXm.so.8.0.0` hash was
`1a38e360f82964e7a318bc61099f7300be6c05ec5a75c3e09814646f9adf545`; the
probe source hash was
`28c60357f3e2c80244f6107eb2ff21a3e7fa13e96696fb75a88b347f280ca631`.
Cross-process multiple targets, incremental payload assembly and complete
application workflows remain open.
