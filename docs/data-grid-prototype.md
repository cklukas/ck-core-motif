# WP-27 DataGrid/TableView prototype

This document records the prototype contract before a public `XmDataGrid`
widget is considered.  The prototype is an installed-prefix consumer fixture;
it uses only public Xt/Motif APIs and does not inspect `XmContainer`, `XmList`,
or any other private widget record.

## Model contract

Rows and columns have stable, application-owned 64-bit IDs.  A model owns its
labels and cell values for the duration of a view snapshot; the view copies
visible text into its bounded viewport and never retains a borrowed pointer
after a model mutation.  The prototype deliberately excludes formulas,
charts, arbitrary data binding, and background model callbacks.

The model reports column headers, row/column counts, cell text, editability,
and errors.  Sorting is a request (`column_id`, ascending/descending), not a
view-side reorder.  Selection is one row and one optional cell.  Edits have an
explicit begin/set/commit/cancel sequence; a failed commit leaves the prior
value intact and returns an application error string.

## Bounded viewport

The prototype uses a fixed 24-pixel header, 22-pixel rows, a maximum of 4096
realized rows, and a maximum of 256 columns.  `top_row` is clamped to the
model's last possible viewport origin.  Geometry is computed from the public
drawing-area width and height; only rows intersecting the viewport are drawn.
The scroll bar is an ordinary public `XmScrollBar` whose value is the bounded
top-row index.

Keyboard focus is owned by the drawing area.  Up/Down move the selected row,
Left/Right move the selected column, Home/End move to the first/last bounded
row, and Return begins or commits an edit.  The prototype records these
transitions but does not yet promise a final resource or translation surface.

## Accessibility

The fixture registers a provider through `XmAccessibilityAdd`.  The table
provider exposes stable row/column counts and cell IDs; each cell provider
reports `XmACCESSIBLE_TABLE_CELL` and its current text.  No row-number or
column-number is used as a semantic identity.  This verifies the table
semantic contract without coupling a future widget release to provider
implementation details.

## Prototype limits and decision gate

The prototype is intentionally single-threaded and bounded.  It does not
attempt lazy providers, arbitrary variable-height rows, frozen panes, merged
cells, formulas, or general data binding.  WP-27 is complete only after the
fixture and benchmark corpus establish memory, redraw, geometry, and callback
behavior for small, typical, and maximum bounded tables, followed by an
explicit go/no-go decision in this document.  A go decision authorizes WP-28;
it does not itself add a public DataGrid class.

## Benchmark evidence

The companion benchmark performs 10,000 selection/layout/edit iterations and
scans every cell in the bounded visible viewport.  It was compiled with
`-std=c99 -O2 -Wall -Wextra -Werror` and run on the AArch64 `uconsole` on
2026-10-25:

```text
case,rows,columns,model_bytes,init_us,operations_us,visible_rows,maxrss_kib
small,64,8,16896,64,5275,28,12080
typical,512,16,266240,957,10536,28,12080
maximum,4096,32,4227072,15284,20917,28,12080
```

The microsecond timings are one clean-run sample and may vary with host load;
the bounded model-size and viewport-row columns are the contract evidence.

The model storage scales linearly with the bounded row/column product.  The
maximum case remains a 4 MiB model snapshot and a 28-row viewport; operation
time rises with visible-column work rather than with unrealized rows.  The
fixture separately verifies geometry, edit lifecycle, sorting requests,
public scroll-bar ownership, and table/cell accessibility IDs.

## WP-27 decision

**Go for WP-28, with restrictions.**  Release work may proceed for a bounded
single-threaded table model using stable application-owned row/column IDs,
fixed or explicitly measured row heights, bounded viewport realization,
request-only sorting, explicit edit commit/cancel/error paths, and provider
backed table/cell accessibility.  Lazy providers, variable-height rows,
frozen panes, merged cells, formulas, charts, and general data binding remain
out of scope and must not be implied by the public API.
