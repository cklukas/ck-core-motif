# Context-menu entry decision

## Scope

WP-39 asks whether Motif needs a new context-menu entry abstraction for
icon-and-shortcut actions.  This package covers entry presentation and
dispatch only; it does not change drag-and-drop ownership, menu admission or
application command routing.

## Evidence

The existing public contract already provides the required pieces:

- `XmCreatePopupMenu` creates a normal popup menu using the existing
  `XmRowColumn` menu behavior;
- `XmPushButton` entries accept `XmNlabelPixmap` for an icon and
  `XmNlabelString` for the visible label;
- `XmNaccelerator` installs the keyboard binding and `XmNacceleratorText`
  displays its human-readable form; and
- `XmNactivateCallback` dispatches the selected entry to application-owned
  code.

The installed consumer fixture `check-context-menu-contract.c` creates these
public widgets, verifies the label, icon, accelerator and displayed shortcut,
and invokes the callback.  It passes 9 checks repeatedly on the qualified
AArch64 `uconsole` prefix under Xvfb.

## Decision

No new context-menu widget or parallel entry API is justified.  A new wrapper
would duplicate existing RowColumn/PushButton semantics, create another
shortcut ownership contract and risk diverging from Motif menu behavior.
Applications that need a higher-level command model can continue to use the
documented `XmCommandPalette` or `XmToolbar` contracts while constructing
ordinary context menus from the public Motif menu and button APIs.

This is a completed no-go decision for the WP-39 context-menu sub-item.  It
does not restrict future additive APIs if an installed consumer demonstrates a
requirement that the existing menu contract cannot express.
