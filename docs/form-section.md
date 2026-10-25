# FormSection

`XmFormSection` is a presentation-only manager with an owned header, an
application-owned content host, and an optional footer.  Create application
children only under `XmFormSectionGetContent`; the section owns its internal
header/content/footer children and performs normal Xt geometry negotiation.

`XmNformSectionHeader` and `XmNformSectionFooter` are copied `XmString`
values.  Margin and spacing resources are explicit and scale through normal
Motif dimension conversion.  A missing footer remains unmapped, and narrow
widths clamp child geometry without changing content ownership or managed
states.  The section has no form-data model, validation behavior, focus trap,
or implicit overlay.

Accessibility exposes the section as a panel named from its header.  The
installed fixture covers header/footer ownership, content-child lifetime,
narrow geometry, accessibility, and destruction.
