---
nav_exclude: true
---
# Rendition converter property header

`XmRenderTableCvtToProp` now writes its property-name header directly into its
checked, dynamically growing conversion result. It no longer concatenates the
names through a fixed process-global buffer, so adding or extending property
names cannot overflow that intermediate state. Converter syntax and public
output remain unchanged.

The focused regression is
`tests/automated/check-render-table-property-header.py`.
