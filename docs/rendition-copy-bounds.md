# Rendition tag-copy allocation bounds

`_XmRenditionCopy` duplicates the optional tag array attached to a rendition.
The copy now checks the tag count against both `size_t` multiplication and
the Xt `Cardinal` allocation width before allocating. An unrepresentable
count follows the existing copy-failure cleanup path and returns `NULL`.
