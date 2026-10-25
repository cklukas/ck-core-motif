# Transactional tab-list replacement

`XmTabListReplacePositions` now copies the source list before applying
replacements. A failed replacement-tab allocation releases the partial copy
and returns the original list unchanged. Successful replacement keeps the
existing ring structure and replaces the original list only after all requested
positions have been processed.
