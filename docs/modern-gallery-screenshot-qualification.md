# Gallery screenshot qualification

`tests/automated/run-gallery-screenshots.py` is the display-backed capture
gate for the component gallery. It is intentionally separate from the native
interaction test: a component must both remain interactive and produce a
valid image through the X11 capture path.

For every manifest entry, the runner starts the verified gallery binary with
`--component ID --screenshot-mode`, locates the mapped `ckmotif-gallery`
top-level window with `xwininfo`, captures that window with `xwd`, and converts
the native capture to PNG with `xwdtopnm` and `pnmtopng`. It selects the
largest mapped window with that exact title and ignores the 1x1 withdrawn
placeholder that some window managers expose during shell startup.

The runner terminates each child and drains its diagnostic pipe before moving
to the next entry. If a component exits before its window appears, the
receipt includes its exit status and stderr; this keeps a toolkit failure
visible instead of misreporting it as a missing display.

Each output directory contains a `receipt.json` with the manifest and binary
hashes, capture-tool paths, per-entry XWD/PNG hashes, elapsed time, and an
explicit result:

- `pass`: every manifest scenario produced both captures;
- `skip`: the binary, display, or capture tools were unavailable;
- `fail`: a scenario or conversion failed, with `reason` and
  `current_entry` identifying the boundary.

On Linux, a qualification run can use a task-owned display and output path:

```sh
DISPLAY=:121 python3 tests/automated/run-gallery-screenshots.py \
  --binary /absolute/path/to/ckmotif-gallery \
  --out /tmp/ckmotif-gallery-screenshots
```

On macOS, output must be below `/Volumes/S8T/tmp` or
`/Volumes/PRO-BLADE/tmp` in accordance with the repository temporary-workspace
policy. The generated PNGs are publication inputs only after the receipt has
`result: "pass"` and its binary hash has been recorded with the documentation
build evidence.

The source-level safety contract is checked by
`tests/automated/check-gallery-screenshot-runner.py`.
