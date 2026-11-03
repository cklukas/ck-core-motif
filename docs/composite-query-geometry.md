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
