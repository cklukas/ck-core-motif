# Label gadget accelerator allocation

Menu label gadgets now replace their private accelerator copy only after
`XtNewString` succeeds.  If a replacement cannot be allocated, the existing
binding remains installed and the requested value is discarded.  The
accelerator `GetValues` hook returns `NULL` directly when no accelerator is
present, avoiding a null string copy.

Successful replacements, non-menu gadgets, and the public LabelGadget ABI are
unchanged.
