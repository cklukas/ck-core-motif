/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifndef _XmSymbolAliasI_h
#define _XmSymbolAliasI_h

/* Aliases must name the same code or storage, including class records.
 * Mach-O does not support the compiler's ELF alias attribute. Define the
 * alternate symbols in the owning object so both archives and dylibs export
 * them without copied records or forwarding functions.
 */
#if defined(__APPLE__) && defined(__MACH__)
#define _XmDefineAlias(old_name, target) \
    extern __typeof__(target) old_name; \
    __asm__(".globl _" #old_name "\n" \
            ".weak_definition _" #old_name "\n" \
            ".set _" #old_name ", _" #target "\n")
#elif defined(__GNUC__) && !defined(NO_WEAK_ALIASES)
#define _XmDefineAlias(old_name, target) \
    extern __typeof__(target) old_name \
        __attribute__((__weak__, __alias__(#target)))
#else
#error "This compiler/object format needs a symbol alias implementation"
#endif

#endif
