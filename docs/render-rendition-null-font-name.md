# Rendition cleanup with no font name

Renditions may intentionally omit `XmNfontName`, for example when an
application supplies an already-loaded font object or defers font loading.
Cleanup must preserve that valid state.  The resource normalizer now checks
for a null font name before comparing the `XmAS_IS` marker string, avoiding a
null-pointer dereference while retaining the existing handling of marker
values and copied names.
