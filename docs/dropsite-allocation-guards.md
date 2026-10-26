---
nav_exclude: true
---
# Drop-site record allocation guards

Drop-site manager bookkeeping creates regions and private records for shell
placeholders, clipping nodes, and registered drop sites.  Those allocations
are now checked before the records are initialized.  Failed region creation
or record allocation releases any partial region and stops the update cleanly;
failed shell and clipper creation no longer reaches registration or child-link
macros with a null record.  Existing drop-site behavior is unchanged when the
allocations succeed.
