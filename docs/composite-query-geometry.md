---
nav_exclude: true
---
# Composite geometry replies

Sidebar, Toolbar, StatusBar, BreadcrumbBar, and CommandPalette calculate their
same existing natural width and height.  Their `QueryGeometry` methods now use
`XmeReplyToQueryGeometry` to compare those values against the request-mode
bits supplied by the parent.  Width-only, height-only, and query-only
negotiations therefore no longer depend on unrequested fields in an
`XtWidgetGeometry` record.

The same request-mode-aware reply contract also applies to Disclosure,
EmptyState, Flyout, FormSection, RowActions, and Toast.  Their existing
natural dimensions and popup sizing policies remain unchanged.
