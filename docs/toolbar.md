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

<!-- gallery-demo-tabs: begin -->
## Component demo

Captured gallery example and its corresponding source excerpt.

::: tabs
== Demo

![xmToolbarWidgetClass demo](gallery-screenshots/xm-toolbarwidgetclass.png)

== Source

Actual gallery excerpt: class = xmToolbarWidgetClass.
The host supplies parent, name, callback and client_data.
Shared factory locals: Widget example; XmString label = NULL.
The shared factory frees label, attaches callbacks and sizes the preview.
Helpers and the complete factory are in gallery_examples.c.

<<< @/demos/programs/ckmotif-gallery/gallery_examples.c#L595-L622

[Complete demo source](../demos/programs/ckmotif-gallery/gallery_examples.c) · [Public header](../lib/Xm/Toolbar.h)

:::
<!-- gallery-demo-tabs: end -->
