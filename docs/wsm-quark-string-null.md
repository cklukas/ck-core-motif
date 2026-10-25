---
nav_exclude: true
---

# WSM configuration quark serialization

Configuration-format packing now converts quarks through a null-safe helper.
If an application supplies an invalid quark, sizing and packing both use an
empty string instead of calling `strlen` or `PackString` with a null pointer.
The valid-quark wire representation is unchanged.
