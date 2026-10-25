# Badge

`XmBadge` is a non-interactive compact status presentation.  Set
`XmNbadgeCount` to a non-negative value for a bounded count, or leave it at
`-1` and provide the copied `XmNbadgeLabel` text.  Counts above
`XmNbadgeMaximum` render as `maximum+`; `XmBadgeGetDisplayedCount` returns the
clamped value, or `-1` for label mode.  `XmNbadgeSeverity` is one of
`XmBADGE_INFO`, `XmBADGE_SUCCESS`, `XmBADGE_WARNING` and `XmBADGE_ERROR`.

Severity is a semantic state, not a colour-only convention.  The badge never
attaches to a host widget, changes host geometry, receives keyboard focus, or
emits callbacks.  Accessibility exposes label-mode text or the bounded count
as a read-only value.
