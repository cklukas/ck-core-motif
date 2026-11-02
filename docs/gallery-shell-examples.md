# Gallery shell examples

The Dialog Shell page opens a populated, modeless dialog owned by the gallery's
top-level window. Its Form child supplies a heading, explanatory text, a Close
action and the Dialog Shell positioning contract. The child starts unmanaged;
opening manages it, and Close or Escape unmanages it. Reopening reuses the same
window and centers it over the gallery, subject to screen-edge clamping.

Construct a Dialog Shell with a dialog-savvy manager child, such as Form or
BulletinBoard, and manage that child to open the dialog. A plain label plus a
separate `XtPopup` does not demonstrate that lifecycle or its default placement.
The example's transient owner is the gallery shell, rather than its launcher
button. These are gallery construction changes; the library's application
positioning and shell APIs are unchanged.

The launcher is constructed with its complete label before initial geometry
negotiation. Its text, margins and focus frame must fit after palette, density
and scale changes.

To open this example immediately for manual inspection:

```sh
ckmotif-gallery --component xm.dialogshellwidgetclass --open-example
```

Native acceptance exercises the actual gallery page and its launcher/Close
callbacks on an isolated display:

```sh
python3 tests/automated/run-gallery-dialog-shell.py \
  --build /absolute/build --results /absolute/fresh-results
```

`--integration-shells` checks Classic, modern light and modern dark, multiple
scales/densities, launcher bounds, visible rendered content, child bounds,
transient ownership, centered placement and repeated close/reopen cycles.
The gallery builds this verifier with strict warning checks; keep the horizontal
and vertical screen-edge clamps as distinct statements so both the compiler and
reviewers see the independent bounds clearly.
