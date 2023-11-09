#ifndef DNLOAD_H
#define DNLOAD_H

/// \file
/// \brief Dynamic loader header stub.
///
/// This file was automatically generated by 'dnload.py'.

#if defined(WIN32)
/// \cond
#define _USE_MATH_DEFINES
#define NOMINMAX
/// \endcond
#else
/// \cond
#define GL_GLEXT_PROTOTYPES
/// \endcond
#endif

#if defined(__cplusplus)
#include <cstdint>
#else
#include <stdint.h>
#endif

#if defined(__cplusplus)
#include <cstdio>
#include <cstdlib>
#else
#include <stdio.h>
#include <stdlib.h>
#endif

/// Macro stringification helper (adds indirection).
#define DNLOAD_MACRO_STR_HELPER(op) #op
/// Macro stringification.
#define DNLOAD_MACRO_STR(op) DNLOAD_MACRO_STR_HELPER(op)

#if defined(DNLOAD_GLESV2)
/// Apientry definition (OpenGL ES 2.0).
#define DNLOAD_APIENTRY GL_APIENTRY
#else
/// Apientry definition (OpenGL).
#define DNLOAD_APIENTRY GLAPIENTRY
#endif

#if (defined(_LP64) && _LP64) || (defined(__LP64__) && __LP64__)
/// Size of pointer in bytes (64-bit).
#define DNLOAD_POINTER_SIZE 8
#else
/// Size of pointer in bytes (32-bit).
#define DNLOAD_POINTER_SIZE 4
#endif

#if !defined(USE_LD)
/// Error string for when assembler exit procedure is not available.
#define DNLOAD_ASM_EXIT_ERROR "no assembler exit procedure defined for current operating system or architecture"
/// Perform exit syscall in assembler.
static void asm_exit(void)
{
#if defined(DNLOAD_NO_DEBUGGER_TRAP)
#if defined(__x86_64__)
#if defined(__FreeBSD__)
    asm("syscall" : /* no output */ : "a"(1) : /* no clobber */);
#elif defined(__linux__)
    asm("syscall" : /* no output */ : "a"(60) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#elif defined(__i386__)
#if defined(__FreeBSD__) || defined(__linux__)
    asm("int $0x80" : /* no output */ : "a"(1) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#elif defined(__aarch64__)
#if defined(__linux__)
    register int x8 asm("x8") = 93;
    asm("svc #0" : /* no output */ : "r"(x8) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#elif defined(__arm__)
#if defined(__linux__)
    register int r7 asm("r7") = 1;
    asm("swi #0" : /* no output */ : "r"(r7) : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#else
#if defined(__x86_64__) || defined(__i386__)
    asm("int $0x3" : /* no output */ : /* no input */ : /* no clobber */);
#elif defined(__aarch64__)
    asm("brk #1000" : /* no output */ : /* no input */ : /* no clobber */);
#elif defined(__arm__)
    asm(".inst 0xdeff" : /* no output */ : /* no input */ : /* no clobber */);
#else
#pragma message DNLOAD_MACRO_STR(DNLOAD_ASM_EXIT_ERROR)
#error
#endif
#endif
    __builtin_unreachable();
}
#endif

#if defined(USE_LD)
/// \cond
#define dnload_realloc realloc
#define dnload_free free
#define dnload_putchar putchar
/// \endcond
#else
/// \cond
#define dnload_realloc g_symbol_table.df_realloc
#define dnload_free g_symbol_table.df_free
#define dnload_putchar g_symbol_table.df_putchar
/// \endcond
/// Symbol table structure.
///
/// Contains all the symbols required for dynamic linking.
static struct SymbolTableStruct
{
    void* (*df_realloc)(void*, size_t);
    void (*df_free)(void*);
    int (*df_putchar)(int);
} g_symbol_table =
{
    (void* (*)(void*, size_t))0xb1ae4962,
    (void (*)(void*))0xc23f2ccc,
    (int (*)(int))0xcde5f545,
};
#endif

#if defined(USE_LD)
/// \cond
#define dnload()
/// \endcond
#else
/// SDBM hash function.
///
/// \param op String to hash.
/// \return Full hash.
static uint32_t dnload_hash(const uint8_t *op)
{
    uint32_t ret = 0;
    for(;;)
    {
        uint32_t cc = *op++;
        if(!cc)
        {
            return ret;
        }
        ret = ret * 65599 + cc;
    }
}
#if defined(__FreeBSD__)
#include <sys/link_elf.h>
#elif defined(__linux__)
#include <link.h>
#else
#error "no elf header location known for current platform"
#endif
#if (8 == DNLOAD_POINTER_SIZE)
/// Elf header type.
typedef Elf64_Ehdr dnload_elf_ehdr_t;
/// Elf program header type.
typedef Elf64_Phdr dnload_elf_phdr_t;
/// Elf dynamic structure type.
typedef Elf64_Dyn dnload_elf_dyn_t;
/// Elf symbol table entry type.
typedef Elf64_Sym dnload_elf_sym_t;
/// Elf dynamic structure tag type.
typedef Elf64_Sxword dnload_elf_tag_t;
#else
/// Elf header type.
typedef Elf32_Ehdr dnload_elf_ehdr_t;
/// Elf program header type.
typedef Elf32_Phdr dnload_elf_phdr_t;
/// Elf dynamic structure type.
typedef Elf32_Dyn dnload_elf_dyn_t;
/// Elf symbol table entry type.
typedef Elf32_Sym dnload_elf_sym_t;
/// Elf dynamic structure tag type.
typedef Elf32_Sword dnload_elf_tag_t;
#endif
/// Get dynamic section element by tag.
///
/// \param dyn Dynamic section.
/// \param tag Tag to look for.
/// \return Pointer to dynamic element.
static const dnload_elf_dyn_t* elf_get_dynamic_element_by_tag(const void *dyn, dnload_elf_tag_t tag)
{
    const dnload_elf_dyn_t *dynamic = (const dnload_elf_dyn_t*)dyn;
    do {
        ++dynamic; // First entry in PT_DYNAMIC is probably nothing important.
    } while(dynamic->d_tag != tag);
    return dynamic;
}
#if defined(DNLOAD_NO_FIXED_R_DEBUG_ADDRESS) || defined(DNLOAD_SAFE_SYMTAB_HANDLING)
/// Get the address associated with given tag in a dynamic section.
///
/// \param dyn Dynamic section.
/// \param tag Tag to look for.
/// \return Address matching given tag.
static const void* elf_get_dynamic_address_by_tag(const void *dyn, dnload_elf_tag_t tag)
{
    const dnload_elf_dyn_t *dynamic = elf_get_dynamic_element_by_tag(dyn, tag);
    return (const void*)dynamic->d_un.d_ptr;
}
#endif
#if !defined(DNLOAD_NO_FIXED_R_DEBUG_ADDRESS)
/// Link map address, fixed location in ELF headers.
extern const struct r_debug *dynamic_r_debug __attribute__((aligned(1)));
#endif
/// Get the program link map.
///
/// \return Link map struct.
static const struct link_map* elf_get_link_map()
{
#if defined(DNLOAD_NO_FIXED_R_DEBUG_ADDRESS)
    // ELF header is in a fixed location in memory.
    const void* ELF_BASE_ADDRESS = (const void*)(
#if defined(__arm__)
            0x10000
#elif defined(__i386__)
            0x2000000
#else
#if (8 != DNLOAD_POINTER_SIZE)
#error "no base address known for current platform"
#endif
            0x400000
#endif
            );
    // First program header is located directly afterwards.
    const dnload_elf_ehdr_t *ehdr = (const dnload_elf_ehdr_t*)ELF_BASE_ADDRESS;
    const dnload_elf_phdr_t *phdr = (const dnload_elf_phdr_t*)((size_t)ehdr + (size_t)ehdr->e_phoff);
    do {
        ++phdr; // Dynamic header is probably never first in PHDR list.
    } while(phdr->p_type != PT_DYNAMIC);
    // Find the debug entry in the dynamic header array.
    {
        const struct r_debug *debug = (const struct r_debug*)elf_get_dynamic_address_by_tag((const void*)phdr->p_vaddr, DT_DEBUG);
        return debug->r_map;
    }
#else
    return dynamic_r_debug->r_map;
#endif
}
/// Return pointer from link map address.
///
/// \param lmap Link map.
/// \param ptr Pointer in this link map.
static const void* elf_transform_dynamic_address(const struct link_map *lmap, const void *ptr)
{
#if defined(__linux__)
    // Addresses may also be absolute.
    if(ptr >= (void*)(size_t)lmap->l_addr)
    {
        return ptr;
    }
#endif
    return (uint8_t*)ptr + (size_t)lmap->l_addr;
}
#if defined(DNLOAD_SAFE_SYMTAB_HANDLING)
/// Get address of one dynamic section corresponding to given library.
///
/// \param lmap Link map.
/// \param tag Tag to look for.
/// \return Pointer to given section or NULL.
static const void* elf_get_library_dynamic_section(const struct link_map *lmap, dnload_elf_tag_t tag)
{
    const void* ptr = elf_get_dynamic_address_by_tag((const dnload_elf_dyn_t*)(lmap->l_ld), tag);
    return elf_transform_dynamic_address(lmap, ptr);
}
#endif
/// Find a symbol in any of the link maps.
///
/// Should a symbol with name matching the given hash not be present, this function will happily continue until
/// we crash. Size-minimal code has no room for error checking.
///
/// \param hash Hash of the function name string.
/// \return Symbol found.
static void* dnload_find_symbol(uint32_t hash)
{
    const struct link_map* lmap = elf_get_link_map();
#if defined(__linux__) && (8 == DNLOAD_POINTER_SIZE)
    // On 64-bit Linux, the second entry is not usable.
    lmap = lmap->l_next;
#endif
    for(;;)
    {
        // First entry is this object itself, safe to advance first.
        lmap = lmap->l_next;
        {
#if defined(DNLOAD_SAFE_SYMTAB_HANDLING)
            const dnload_elf_sym_t* symtab = (const dnload_elf_sym_t*)elf_get_library_dynamic_section(lmap, DT_SYMTAB);
            const char* strtab = (char*)elf_get_library_dynamic_section(lmap, DT_STRTAB);
            const dnload_elf_sym_t* symtab_end = (const dnload_elf_sym_t*)strtab;
            // If the section immediately following SYMTAB is not STRTAB, it may be something else.
            {
                const dnload_elf_sym_t *potential_end = (const dnload_elf_sym_t*)elf_get_library_dynamic_section(lmap, DT_VERSYM);
                if(potential_end < symtab_end)
                {
                    symtab_end = potential_end;
                }
            }
#else
            // Assume DT_SYMTAB dynamic entry immediately follows DT_STRTAB dynamic entry.
            // Assume DT_STRTAB memory block immediately follows DT_SYMTAB dynamic entry.
            const dnload_elf_dyn_t *dynamic = elf_get_dynamic_element_by_tag(lmap->l_ld, DT_STRTAB);
            const char* strtab = (const char*)elf_transform_dynamic_address(lmap, (const void*)(dynamic->d_un.d_ptr));
            const dnload_elf_sym_t *symtab_end = (const dnload_elf_sym_t*)strtab;
            ++dynamic;
            const dnload_elf_sym_t *symtab = (const dnload_elf_sym_t*)elf_transform_dynamic_address(lmap, (const void*)(dynamic->d_un.d_ptr));
#endif
            for(const dnload_elf_sym_t *sym = symtab; (sym < symtab_end); ++sym)
            {
                const char *name = strtab + sym->st_name;
                if(dnload_hash((const uint8_t*)name) == hash)
                {
                    void* ret_addr = (void*)((const uint8_t*)sym->st_value + (size_t)lmap->l_addr);
#if defined(__linux__) && (defined(__aarch64__) || defined(__i386__) || defined(__x86_64__))
                    // On Linux and various architectures, need to check for IFUNC.
                    if((sym->st_info & 15) == STT_GNU_IFUNC)
                    {
                        ret_addr = ((void*(*)())ret_addr)();
                    }
#endif
                    return ret_addr;
                }
            }
        }
    }
}
/// Perform init.
///
/// Import by hash - style.
static void dnload(void)
{
    unsigned ii;
    for(ii = 0; (3 > ii); ++ii)
    {
        void **iter = ((void**)&g_symbol_table) + ii;
        *iter = dnload_find_symbol(*(uint32_t*)iter);
    }
}
#endif

#if defined(__clang__)
/// Visibility declaration for symbols that require it (clang).
#define DNLOAD_VISIBILITY __attribute__((visibility("default")))
#else
/// Visibility declaration for symbols that require it (gcc).
#define DNLOAD_VISIBILITY __attribute__((externally_visible,visibility("default")))
#endif

#if !defined(USE_LD)
#if defined(__cplusplus)
extern "C"
{
#endif
/// Program entry point.
void _start() DNLOAD_VISIBILITY;
#if defined(__cplusplus)
}
#endif
#endif
#endif

