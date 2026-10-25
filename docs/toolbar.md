# Toolbar

`XmToolbar` is an opt-in client-area action strip.  The application supplies
stable action IDs, labels, optional shortcut text, priority and an explicit
`overflowable` flag.  The toolbar copies the IDs and `XmString` values; it
does not own the application's action model or `user_data`.

When space is constrained, non-overflowable actions remain in the strip and
overflowable actions are selected by descending priority.  Remaining actions
are placed in an ordinary Motif pulldown menu behind the `More` cascade
(`XmNtoolbarOverflowLabel`), and `XmToolbarGetOverflowCount` reports how many
are there.  The toolkit preserves action IDs and dispatches both the strip and
menu entries through `XmNtoolbarActivateCallback`; shortcut text is retained
and shown with the action label, but applications remain responsible for
global accelerator policy.

The toolbar owns only its push-button and menu-entry presentation children.
It does not create a navigation model, launch commands, or change existing
menus and toolbar widgets.  Accessibility exposes a named toolbar panel and
reports one action when an overflow menu is available.
