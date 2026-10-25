# Gallery screenshot qualification

`tests/automated/run-gallery-screenshots.py` is the display-backed capture
gate for the component gallery. It is intentionally separate from the native
interaction test: a component must both remain interactive and produce a
valid image through the X11 capture path.

For every manifest entry, the runner starts the verified gallery binary with
`--component ID --screenshot-mode --appearance PROFILE --scale SCALE`, locates the mapped `ckmotif-gallery`
top-level window with `xwininfo`, captures that window with `xwd`, and converts
the native capture to PNG with `xwdtopnm` and `pnmtopng`. It selects the
largest mapped window with that exact title and ignores the 1x1 withdrawn
placeholder that some window managers expose during shell startup.

`PROFILE` is one of `classic`, `modern-light`, `high-contrast` or
`modern-dark`. `SCALE` is a finite positive display-scale override no greater
than 64. The gallery applies these through the public
`XmApplyAppearanceProfile` and `XmSetDisplayScaleOverride` APIs after
realization; a failed transaction terminates the scenario instead of
producing an unlabeled image. The runner records both values in
`receipt.json`, so captures at different settings cannot be confused with
the default profile.

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
  --out /tmp/ckmotif-gallery-screenshots \
  --appearance modern-dark --scale 2.0
```

On macOS, output must be below `/Volumes/S8T/tmp` or
`/Volumes/PRO-BLADE/tmp` in accordance with the repository temporary-workspace
policy. The generated PNGs are publication inputs only after the receipt has
`result: "pass"` and its binary hash has been recorded with the documentation
build evidence.

The source-level safety contract is checked by
`tests/automated/check-gallery-screenshot-runner.py`.

The complete profile/scale capture is orchestrated by
`tests/automated/run-gallery-screenshot-matrix.py`. Its default matrix is the
four public appearance profiles crossed with scales 1.0, 1.5, 2.0 and 3.0;
`--appearances` and `--scales` accept explicit comma-separated subsets. Each
cell is a separate child receipt and the matrix fails or skips as soon as a
child does, while retaining the completed cells and their exact commands.
By default the XWD/PNG captures are retained. On constrained qualification
hosts, `--discard-captures` removes only those generated image files after
their hashes are recorded in the child receipt; the matrix receipt records
that the run was hash-only rather than presenting it as publication imagery.
The source-level matrix contract is checked by
`tests/automated/check-gallery-screenshot-matrix.py`.

For example, a full Linux qualification uses a fresh output directory:

```sh
DISPLAY=:121 python3 tests/automated/run-gallery-screenshot-matrix.py \
  --binary /absolute/path/to/ckmotif-gallery \
  --out /tmp/ckmotif-gallery-screenshot-matrix
```

On 2026-10-25, the repaired installed AArch64 `uconsole` prefix passed the
default 16-cell matrix (four profiles crossed with 1.0, 1.5, 2.0 and 3.0)
across all 94 manifest entries: 1,504 captures, 471.628 seconds, and no
failed or skipped child receipts. The matrix used `--discard-captures`, so
only hashes and receipts were retained at
`/tmp/ckmotif-gallery-screenshot-matrix-20261025-r2/`; this is qualification
evidence, not publication imagery. The gallery binary fingerprint was
`bcd9940a094352c0862887397300e675c059eae77138ac9fe2f5a94c30a63602` and the
installed `libXm.so.8.0.0` fingerprint was
`3fe2f3d43371e8cb4639a50b04c5ead68aa6fb735d85d6f115958c6bb96ea5c9`.

Published PNGs are projected into `docs/gallery-screenshots/manifest.json` by
`tests/automated/generate-gallery-screenshot-manifest.py`. The companion
`tests/automated/check-gallery-screenshot-assets.py` verifies the manifest
hash, binary fingerprint, profile/DPI capture labels, PNG dimensions, file
hashes, and catalogue links. This prevents a locally replaced image from
silently becoming gallery evidence.
