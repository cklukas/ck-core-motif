---
nav_exclude: true
---
# Mrm interface-module null guards

Interface-module readers now reject a valid resource context whose data buffer
is null before reading `RGMModuleDesc.validation`. Shared module-binding and
direct module-get paths return `MrmBAD_IF_MODULE` for empty data instead of
dereferencing a null pointer.
