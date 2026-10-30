---
nav_exclude: true
---
# FontSelector family refresh guards

Family-list refreshes now validate selector/font/family state, reject negative
or missing family storage, and stop safely when the temporary “any” value cannot
be converted. Valid refreshes retain their existing filtering and selection
behavior.
