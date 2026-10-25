# Sidebar

`XmSidebar` is an opt-in bounded client-area composite.  Applications create
their content below `XmSidebarGetContent`; the sidebar owns that content host
and its toggle presentation, while the application owns the content widgets,
navigation model and selection policy.

The expanded and collapsed widths are controlled by
`XmNsidebarExpandedWidth` and `XmNsidebarCollapsedWidth`.  `XmSidebarToggle`
and the toggle button provide the same state transition, and `Ctrl+M` is the
keyboard equivalent when focus is anywhere in the sidebar subtree.  If focus
is inside the content host when it collapses, the sidebar moves focus to its
toggle and restores the prior content focus when expanded again.

The composite exposes a named accessible panel with one toggle action.  It does
not create an adaptive navigation stack, infer application routes, or alter
window-manager decorations.
