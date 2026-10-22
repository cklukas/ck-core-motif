/* Installed widget interfaces must be usable by strict modern C++ clients.
 * LGPL-2.0-or-later. Compile with -std=c++20 -Wall -Wextra -Wpedantic -Werror. */
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Accessible.h>
#include <Xm/Disclosure.h>

static_assert(XmABI_VERSION == 5, "This source cohort uses native ABI epoch 5");
int main() { return XmToolkitABI() == XmABI_VERSION ? 0 : 1; }
