# CommandPalette

`XmCommandPalette` is an opt-in, non-modal command chooser.  The application
owns the command model and dispatches the stable `id` received from
`XmNcommandPaletteInvokeCallback`; the toolkit only filters and presents the
model.  `user_data` is borrowed and is returned unchanged with the callback.

`XmCommandPaletteSetItems` copies each item ID, label and shortcut, so callers
may release their input strings after the call.  Filtering is an explicit,
case-insensitive substring match over labels.  Empty or unmatched filters do
not execute commands.  `XmCommandPaletteShow` focuses the search field and
remembers the prior focus; dismissal restores it when that widget is still
managed.  Escape, search activation and list default activation are all
bounded to the instance.

The palette has no process-global command registry, navigation policy or
implicit execution from drawing/filter callbacks.  Accessibility exposes the
instance as a named panel with one explicit invoke action; its child search
field and list retain their native text and selection semantics.  The
installed fixture covers copied model ownership, filtering, dispatch,
accessibility, no-match behavior, dismissal and teardown.
