# Transactional tab-list insertion

`XmTabListInsertTabs` now builds copied tabs as a detached chain. The existing
list is changed only after all requested tabs have been copied successfully.
If a later allocation fails, the partial chain and copied list are released and
the caller receives `NULL`; successful insertion keeps the existing ring and
marked-tab ownership behavior.
