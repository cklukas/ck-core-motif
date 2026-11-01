---
nav_exclude: true
---
# External selection-owner lifecycle

Motif transfer helpers can replace a selection owned by an unrelated X11
window.  The replacement must become the Motif widget's owner, and destroying
that widget must release the selection instead of leaving a stale owner behind.

The focused regression creates a foreign owner through an independent X11
client connection, then claims its named selection through `XmeNamedSource`.
The foreign client takes ownership back and the fixture processes the pending
Xt events. Destroying the former Motif owner must leave the foreign owner
intact. Destroying the foreign window must then clear ownership. A fresh Motif
widget reacquires the selection and releases it on destruction:

```sh
python3 tests/automated/run-external-selection-regressions.py \
  /absolute/prefix /absolute/results
```

The two clients use separate X connections in one process. This is not a
cross-process data-transfer or INCR test. The fixture also retains null-widget
and invalid-selection checks. The runner supplies a fresh Xvfb server and
verifies the actual loaded toolkit with `dladdr`; source, header, library and
executable hashes, commands, timings and results are recorded in `receipt.json`.

On 2026-11-01 all 16 checks passed against the isolated ARM64 ABI9 snapshot
with SHA-256 `cb73f5a93257004b4509b09184782f17a99bdf583bf299cd5765b1540a4faa7a`.
This verifies ownership lifecycle only; external payload formats, cancellation,
INCR transfers and broad application interoperability remain separate gates.
