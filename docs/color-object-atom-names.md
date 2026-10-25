# ColorObj atom-name allocation

ColorObj initialization builds one customization-property name per screen and
interns them in one X request. For displays beyond the inline ten-name buffer,
the names are individually allocated.

The setup now checks the name-vector allocation, tracks each dynamic name, and
releases all names already allocated if a later name fails. Atom interning is
skipped unless the complete vector exists; normal fallback behavior then
continues without dereferencing partial storage.
