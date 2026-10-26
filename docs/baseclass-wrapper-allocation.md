---
nav_exclude: true
---
# Base-class wrapper allocation

Motif installs wrapper records lazily while Xt initializes widget classes.
Those records are allocated independently of the class extension itself.  The
class initializer and leaf-wrapper hooks now stop safely when a wrapper record
cannot be allocated, rather than dereferencing a null record while copying
realize, resize, geometry, or class-part hooks.  Normal wrapper installation
is unchanged when the private records are available.
