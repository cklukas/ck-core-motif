# CK Core Motif component gallery

`ckmotif-gallery` is the maintained native reference application for the
component inventory in [`modern-gallery-manifest.json`](modern-gallery-manifest.json).
The complete manifest-backed component catalogue (105 public classes, including
legacy public widgets) is in
[`modern-gallery-components.md`](modern-gallery-components.md).
The JSON manifest is the source of truth for navigation categories, example
owners, source-fragment IDs, interaction tests, documentation anchors, and
screenshot scenarios; the gallery build rejects incomplete records before it
generates its registry.

The gallery separates application navigation from the selected component's
**Example** and **Source** views. A searchable, resizable sidebar preserves
category disclosure and selection. Component pages use native labels and an
Xft render table, with 24-pixel inner margins, descriptive titles, API names,
and feedback beneath the live example. The view strip exposes **Example**,
**Source**, and **Built-in Icons**; every tab has a one-pixel gray outline when
inactive so the available views remain legible against the page. Source uses a
monospace rendition.

The preview preserves the control's natural proportions. Wide or tall controls
scroll within the preview instead of stretching small badges or clipping a
font selector. The preview tracks subsequent native size changes, including
expanded disclosure content. The classic decorated Notebook remains a populated component
example, rather than framing every page in the application.

The default appearance is `modern-light`. Explicit `--appearance` and `--scale`
options remain available for qualification. Live Theme/DPI controls are only
shown in builds that enable `CKMOTIF_GALLERY_ENABLE_LIVE_TRANSACTIONS`; the
ordinary gallery does not present disabled engineering controls in its header.

The native examples include populated data views, command discovery, meaningful
notifications, date/time values, labeled editing controls, enabled/disabled
buttons, checked/unchecked toggles, and real settings forms inside layout
managers. Popup examples have named triggers. Nonvisual inventory entries
explain their infrastructure role instead of presenting an empty interactive
example. Ordinary controls are operated directly; there is no generic
“Activate example” button.

`XmHierarchy` is classified as a base class: it manages hierarchy nodes but
does not lay them out. Its `XmOutline` and `XmTree` subclasses have populated
live examples. Optional classes absent from a build explain that state instead
of showing an empty preview marked ready.

Source pages are generated from annotated code in
`demos/programs/ckmotif-gallery/gallery_examples.c`, the same construction path
used by the application. The build's `gallery_provenance.json` records the
manifest and example-source hashes. `make check` checks the generated registry,
source fragments and catalogue links. Display-backed integration additionally
exercises component selection, filtering, page switching, callbacks, traversal
and focus restoration.

## Welcome and built-in icons

**Welcome** introduces navigation and keyboard use. **Built-in Icons** shows
the five dialog symbols with readable names. SVG-enabled builds use the
embedded vector artwork described in [SVG images](svg-images.md); other builds
show the original Motif bitmaps.

![Current gallery welcome page](gallery-screenshots/welcome.png)

![Current gallery built-in icon grid](gallery-screenshots/icons.png)

The SVG resource names are `dialog_error`, `dialog_information`,
`dialog_question`, `dialog_warning`, and `dialog_working`. Standard message-box
constructors select them automatically when SVG is enabled, after application
image overrides and before the built-in bitmap fallback.

## Dialog screenshots and constructors

These client-window screenshots are captured from the current SVG-enabled
gallery binary in a display-backed Linux session. Each uses the same public construction pattern; replace the
constructor name and dialog identifier as shown:

| Dialog | Constructor | Current screenshot |
| --- | --- | --- |
| Error | `XmCreateErrorDialog(parent, "errorDialog", args, count)` | ![Error dialog](gallery-screenshots/error.png) |
| Information | `XmCreateInformationDialog(parent, "informationDialog", args, count)` | ![Information dialog](gallery-screenshots/information.png) |
| Question | `XmCreateQuestionDialog(parent, "questionDialog", args, count)` | ![Question dialog](gallery-screenshots/question.png) |
| Warning | `XmCreateWarningDialog(parent, "warningDialog", args, count)` | ![Warning dialog](gallery-screenshots/warning.png) |
| Working | `XmCreateWorkingDialog(parent, "workingDialog", args, count)` | ![Working dialog](gallery-screenshots/working.png) |

For example:

```c
Arg args[2];
Cardinal count = 0;
XmString message = XmStringCreateLocalized("The operation needs attention.");
XtSetArg(args[count], XmNmessageString, message); ++count;
Widget dialog = XmCreateWarningDialog(parent, "warningDialog", args, count);
XmStringFree(message);
XtManageChild(dialog);
```

When available, the accessibility bridge attaches to the realized shell and
its tree role is checked by native integration. Optional capabilities such as
PrintShell retain inventory entries and explain when unavailable.

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

The runner selects each manifest component through `--component`, waits for its
process-ID and idle-readiness properties, then captures that window with `xwd`.
It skips when required runtime tools are absent and fails if a process exits or
never becomes ready. A successful capture is evidence of identity and readiness;
visual review remains required.
Pass `--appearance classic|modern-light|high-contrast|modern-dark` and
`--scale SCALE` to qualify an explicit profile and simulated display scale;
these options use the public toolkit transactions and are recorded in the
capture receipt.

The generated component catalogue links each manifest entry to its captured
PNG under `docs/gallery-screenshots/`. Capture receipts record the binary,
appearance profile, scale and image hashes; XWD files are temporary capture
intermediates.

The [layout and artwork validation record](gallery-ui-validation.md) summarizes
the native acceptance checks and screenshot provenance.
