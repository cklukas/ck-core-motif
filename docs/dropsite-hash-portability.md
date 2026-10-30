# DropSite Manager hash portability

DropSite Manager widget keys now hash through bounded `uintptr_t` arithmetic
before conversion to signed `XmHashValue`. This avoids implementation-defined
pointer narrowing on 64-bit systems while preserving widget identity matching.
