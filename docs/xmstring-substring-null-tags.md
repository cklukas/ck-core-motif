# XmString substring matching with null tags

Substring highlighting can encounter a segment without an explicit rendition
tag.  Matching now normalizes both the searched segment and the underlined
segment to `XmFONTLIST_DEFAULT_TAG` before comparing or checking the current
charset, avoiding null-string operations while preserving default-tag
matching.
