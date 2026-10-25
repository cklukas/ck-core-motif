#!/usr/bin/env python3
"""Check the optional C++ support package has an opt-in install contract."""

from pathlib import Path


root = Path(__file__).parents[2]
configure = (root / "configure.ac").read_text()
makefile = (root / "Makefile.am").read_text()

assert "--enable-cxx-support" in configure
assert "AM_CONDITIONAL([CXX_SUPPORT]" in configure
assert "if CXX_SUPPORT" in makefile
assert "ckmotifxxinclude_HEADERS = cpp/ckmotif++/motif_support.hpp" in makefile
assert "pkgconfig_DATA = ckmotif++-8.pc" in makefile

print("optional C++ support package has an opt-in install contract")
