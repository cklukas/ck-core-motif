# RCMenu excluded-parent-pane growth

Menu popdown temporarily records the active parent panes while restoring
shared or torn-off menus. The ABI stores the pane count and capacity as signed
shorts.

The traversal now uses a checked integer depth and grows the pane array through
a temporary pointer. Capacity and byte-size limits are validated before Xt is
called, and the replacement is published only after success. An allocation
failure stops the traversal with all previously recorded panes intact.
