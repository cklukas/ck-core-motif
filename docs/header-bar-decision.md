# Header-bar decision

WP-40 evaluates a header-bar presentation separately from the bounded
client-area composites.  This package does not change window-manager title-bar
controls, decorations, drag regions, system menus or shell integration.

The decision is **defer**.  Xt and ordinary X11 do not provide a portable
contract for replacing or extending the window-manager frame.  A client-area
header can be composed from existing Motif managers, but calling it a title-bar
replacement would make decoration ownership, hit testing, accessibility and
window-management behavior depend on the selected desktop and compositor.

Reopen this decision only with a versioned server/desktop contract, a real
window evidence matrix and an explicit shell-boundary ownership model.  The
existing `XmToolbar` and `XmSidebar` remain the supported client-area
composition points and do not imply title-bar integration.
