# Tab-list mutation position bounds

`XmTabListReplacePositions` and `XmTabListRemoveTabs` now validate every
caller-supplied position against the copied list before traversing it. An
out-of-range position discards the private copy and returns the original list,
avoiding null dereferences and partial mutation.
