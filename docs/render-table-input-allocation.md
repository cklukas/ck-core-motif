# Render-table input-copy allocation

Render-table resource conversion tokenizes a mutable copy of the input
string. The copy is now checked before tokenization; allocation failure
returns `False` without passing a null pointer into the parser.
