# CK Core Motif component gallery

`ckmotif-gallery` is the maintained native reference application for the
component inventory in [`modern-gallery-manifest.json`](modern-gallery-manifest.json).
The complete manifest-backed component catalogue is in
[`modern-gallery-components.md`](modern-gallery-components.md).
The JSON manifest is the source of truth for navigation categories, example
owners, source-fragment IDs, interaction tests, documentation anchors, and
screenshot scenarios; the gallery build rejects incomplete records before it
generates its registry.

The shell is deliberately assembled from public toolkit interfaces:

- `XmSidebar` owns the collapsible navigation content;
- `XmTreeView` presents manifest categories and component entries;
- `XmNotebook` provides the **Example** and **Source** pages; and
- `XmStatusBar` reports selection, callback, and capability state.

The sidebar is the left pane of a horizontal split: categories and component
entries stay visible on the left while the selected live control occupies the
right pane.

The native integration mode exercises sidebar collapse/restore, manifest
selection across every available constructible entry, notebook page
switching, generated-source lookup, callback delivery, status-bar updates,
keyboard traversal, and focus restoration. Shell-class entries use public
Xt popup-shell construction; ordinary widget and gadget entries use their
manifest-bound public class pointer and invoke the first available semantic
public callback (with a help callback fallback).

When the public accessibility bridge is available, the gallery attaches it to
the realized shell and verifies the navigation tree's semantic role during
the native integration run. The optional AT-SPI transport remains optional;
normal gallery operation does not depend on it.

Source pages are generated from annotated code in
`demos/programs/ckmotif-gallery/gallery_examples.c`. They are not copied UI
strings. The excerpt includes the public construction path, semantic callback
selection (activate/value-change/help fallback), and public `XtDestroyWidget`
teardown used by the live example. The component class pointer, source ID,
interaction ID, documentation anchor, and screenshot ID remain linked to the
same manifest record.

Compound controls are populated with real public models before they are shown.
For example, the segmented-control entry renders three selectable segments —
`Overview`, `Details`, and `Activity` — and selects `Overview`; it is not a
metadata-only placeholder. The same construction block powers the live
component pane and its Source tab.

Each gallery build also emits `gallery_provenance.json`, including the manifest
and annotated-source hashes plus one record for every component. `make check`
rejects stale hashes, missing IDs, duplicate records, or drift in any of the
source/test/docs/screenshot references.

The toolkit now exposes public `XmApplyAppearanceProfile` and display-scale
transaction APIs. The gallery wires its Theme and DPI callbacks to those APIs
behind `CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS`, but the default build keeps
the controls disabled while the WP-05 qualification matrix is incomplete. In
the default build the status output says that qualification is pending; the
gallery never simulates a transition or claims that it works early.

`PrintShell` is emitted behind `PRINTING_SUPPORTED`; a print-disabled build
keeps the inventory and navigation entry but reports the capability as
unavailable instead of referring to a missing class symbol.

The source contract check is:

```sh
python3 tests/automated/check-gallery-source.py
```

The display-backed native interaction check is:

```sh
python3 tests/automated/run-gallery-integration.py
```

It reports a deliberate skip when no display-backed gallery binary is
available. It does not turn the pending live appearance/DPI work into a false
pass.

Screenshot scenarios can be exercised with a verified binary using:

```sh
python3 tests/automated/run-gallery-screenshots.py \
  --binary /absolute/path/to/ckmotif-gallery \
  --out /Volumes/S8T/tmp/ckmotif-gallery-screenshots
```

The runner selects each manifest component through `--component`, captures the
named gallery window with `xwd`, and skips when the required runtime is absent.
Pass `--appearance classic|modern-light|high-contrast|modern-dark` and
`--scale SCALE` to qualify an explicit profile and simulated display scale;
these options use the public toolkit transactions and are recorded in the
capture receipt.

The generated component catalogue links each manifest entry to its captured
PNG under `docs/gallery-screenshots/`. The XWD captures are retained beside
the PNGs as native evidence when the gallery is qualified on a display-backed
host.
