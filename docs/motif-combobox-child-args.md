---
nav_exclude: true
---
# ComboBox child-argument allocation

The optional `FIX_1473` child-argument filter now falls back to the original
argument list if its temporary array cannot be allocated. ComboBox child
creation therefore remains functional under allocation pressure, while the
height-filtering behavior is unchanged when the temporary array is available.
