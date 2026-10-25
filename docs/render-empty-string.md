---
nav_exclude: true
---
# Empty render-table string tokens

The render-table lexer now emits an allocated empty string for `""` and
advances beyond both quote characters.  Empty quoted values therefore no
longer leave the lexer at the same position and repeatedly parse the token.
