---
nav_exclude: true
---
# DataGrid geometry negotiation

`XmDataGrid` uses 480x240 only as its initial fallback allocation.  Its Xt
`QueryGeometry` method now reports the widget's actual current dimensions and
uses `XmeReplyToQueryGeometry` to answer the caller's intended geometry.
Applications can therefore assign an explicit DataGrid size without a parent
query negotiating it back to the fallback dimensions.
