---
nav_exclude: true
---
# FontSelector bold and italic refresh guards

The FontSelector bold/italic refresh now validates its selector state, current
font, family storage, and both toggle children before reading or updating
them. A partially initialized or partially destroyed selector therefore
returns safely instead of dereferencing incomplete widget state.
