# TreeView contract and decision record

`XmTreeView` is an additive model/view widget.  It does not subclass or
inspect `XmTree`, `XmOutline`, `XmContainer`, or any application child widget.
The application supplies a flat array of nodes through
`XmTreeViewSetNodes`; each node has an opaque stable `id`, an optional
`parent_id`, a copied UTF-8 label, and explicit `has_children`/`expanded`
state.  IDs and any application payload they identify remain application
owned.  Labels are copied so a model can retire its input array after the
setter returns.

The model is synchronous and belongs to the Xt application thread.  A setter
must run on that thread; the widget never performs I/O or invokes a provider
from drawing, geometry negotiation, or an event filter.  Replacing the model
preserves selection only when the selected ID is still present.  A node count
above 4096 is rejected without changing the current model.  Duplicate IDs,
cycles, missing parents, and child references whose parent does not advertise
`has_children` are rejected without changing the current model; the widget
never silently reparents malformed data.

The viewport is deliberately bounded.  `XmNtreeViewVisibleRows` and
`XmNtreeViewTopRow` expose the finite visible window; wheel events and
`XmTreeViewSetTopRow` move through the flattened visible rows.  This is a
bounded drawing policy, not a general virtualization engine.  Lazy child
retrieval, incremental model callbacks and remote data loading are deferred
until a separately specified model contract exists.

Left/right keys expand, collapse, or move to a parent/first child according to
the logical direction; Up/Down move through visible rows; Return toggles a
branch.  Pointer selection and disclosure toggling use the same callbacks.
`XmNtreeViewSelectionCallback` and `XmNtreeViewExpansionCallback` report
opaque IDs and callback data that is valid only for the callback duration.
Callbacks may destroy the widget; callers must not use it after returning.

`XmNtreeViewRtl` mirrors indentation and directional keyboard behavior without
reordering the model.  Accessibility exposes the control as a tree with a
selection/value summary.  Per-node semantic children are intentionally not
invented without native child ownership; a later accessibility extension must
define stable child IDs first.

## WP-25 decision

The bounded flat-model contract is accepted for the first additive TreeView
implementation.  It provides deterministic geometry, mutation and lifecycle
behavior while leaving the existing hierarchy widgets untouched.  General
virtualization and asynchronous/lazy providers remain out of scope.
