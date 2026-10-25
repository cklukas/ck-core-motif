# Drop-site lifecycle qualification

`tests/automated/check-dropsite-lifecycle.c` is an installed-prefix consumer
probe for the public Motif drop-site contract. It registers a Label as a drop
site, retrieves the operations, import-target list and drag/drop callbacks,
exercises the documented update bracket, unregisters and re-registers the
site, and destroys a second registered site without an explicit unregister.

Run it against a fresh prefix with:

```sh
python3 tests/automated/run-dropsite-lifecycle.py \
  /absolute/installed/prefix /absolute/fresh/results
```

The AArch64 `uconsole` qualification on 2026-10-25 passed seven checks in
three repeated Xvfb runs. The runner records the source hash, `libXm.so.8.0.0`
hash, loaded-library proof, output path and elapsed time.

This is a lifecycle and registration boundary only. The bounded same-process
gesture and transfer conversion contracts are covered separately by
`docs/dnd-transfer-lifecycle.md`. This probe does not claim cancellation,
owner replacement or a complete application workflow; those remain separate
DnD gates.
