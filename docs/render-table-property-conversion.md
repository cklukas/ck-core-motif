---
nav_exclude: true
---
# Render-table property conversion

Property conversion now rejects a failed `XmRenditionCreate` instead of
inserting a null entry into the rendition array. The temporary array is also
released after conversion, including malformed-input error paths.
