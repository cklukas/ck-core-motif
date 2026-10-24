# BreadcrumbBar

`XmBreadcrumbBar` is an additive, resource-driven path presentation.  The
application owns each segment's label and opaque `id`; `XmBreadcrumbBarSetSegments`
copies labels while retaining IDs for callback delivery.  The widget never
parses paths or navigates on its own.

`XmNbreadcrumbMaxVisible` enables bounded leading-ellipsis collapse, and
`XmNbreadcrumbRtl` changes the visual and keyboard direction without changing
the logical segment order.  Left/right keys move the current segment and
Return activates it.  Pointer selection and activation use the same callbacks.

`XmNbreadcrumbCurrentChangedCallback` reports selection changes and
`XmNbreadcrumbActivateCallback` reports an explicit activation.  Callback data
is an `XmBreadcrumbBarCallbackStruct`; the `id` is the application-owned
payload and must remain valid until the callback returns.  No callback runs
from drawing or geometry negotiation.

The control has no global popup, path model, or external URL behavior.  Empty
segments are allowed and are rendered as empty labels.  Existing Button,
Label, Menu and List contracts are unchanged.
