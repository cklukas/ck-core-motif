# DataField render-table lifetime

DataField copies the render table supplied through `XmNfontList` during
initialization, just as it does for the default table. The copy is therefore
owned by the widget and must be released by `df_Destroy` regardless of where
the source table came from.

The destroy path previously consulted `fontlist_created`, which only recorded
whether the source was the default table. Application-supplied tables were
copied but their private copies were not freed. The destroy path now checks the
owned table itself and releases it, covering both source cases without changing
the public resource or ABI contract.

The strict Unicode/Xft fixture exercises application-supplied tables on
TextField, Text and DataField widgets. Its current remaining sanitizer report
is a separate 104-byte allocation rooted in the system Xt selection-owner
implementation; that dependency report is retained rather than suppressed.
