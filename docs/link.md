# Link

`XmLink` is an application-dispatched link affordance.  The control owns a
copy of its `XmNlinkLabel` and `XmNlinkTarget` values, draws a visible
underlined label, and exposes `XmNlinkVisited` plus
`XmNlinkVisitOnActivate`.  Space, Return, keypad Return, and pointer press
invoke `XmNlinkActivateCallback` with the target payload.  The default visit
policy marks the link visited before the callback; applications may disable
that policy and manage the state themselves.

The widget never launches a URL, opens a file, or invokes an external program.
The application owns dispatch, target interpretation, navigation and lifetime.
Accessibility exposes the link as a one-action button with its label as the
accessible name.  Focus and visited state remain visible without relying on
colour alone.
