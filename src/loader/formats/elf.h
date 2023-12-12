#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>

// segment permissions flag bits 
#define PF_X    0x01 // execute
#define PF_W    0x02 // write
#define PF_R    0x04 // read
#define PF_MASKPROC 0xF0000000 // unspecified

/*
* PROGRAM HEADER DEFINES
*/
// segment types, p_type
#define PT_NULL     0 // array element is unused
#define PT_LOAD     1 // array element describes a loadable segment described by p_filesz and p_memsz
#define PT_DYNAMIC  2 // array element describes dynamic linking information
#define PT_INTERP   3 // array element specifies the location and size of a null-terminated path name to invoke as an interpreter
#define PT_NOTE     4 // array element specifies the location and size of auxiliary information
#define PT_SHLIB    5 // array element reserved but has unspecified semantics
#define PT_PHDR     6 // array element specifies the location and size of the program header table itself


/*
* SECTION HEADER TABLE DEFINES
*/
// segment types, sh_type
#define SHT_NULL     0 // section header is inactive; it does not have an associated section
#define SHT_PROGBITS 1 // section holds information defined by the program, whose format and meaning are determined solely by the program.
#define SHT_SYMTAB   2 // section hold a symbol table
#define SHT_STRTAB   3 // section holds a string table
#define SHT_RELA     4 // section holds relocation entries with explicit addends
#define SHT_HASH     5 // section holds a symbol hash table
#define SHT_DYNAMIC  6 // section holds information for dynamic linking
#define SHT_NOTE     7 // section holds information that marks the file in some way
#define SHT_NOBITS   8 // section of this type occupies no space in the file but otherwise resembles SHT_PROGBITS
#define SHT_REL      9 // section holds relocation entries without explicit addends
#define SHT_SHLIB   10 // section type is reserved but has unspecified semantics
#define SHT_DYNSYM  11 // section hold a symbol table
#define SHT_LOPROC  12 // values between low and high are reserved for processor-specific semantics
#define SHT_HIPROC  13 
#define SHT_LOUSER  14 // value specifies the lower bound of the range of indexes reserved for application programs
#define SHT_HIUSER  15 // value specifies the upper bound of the range of indexes reserved for application programs

/*
* ELF HEADER DEFINES
*/
// elf header types, e_type
#define ET_NONE 0 // No file type
#define ET_REL  1 // Relocatable file
#define ET_EXEC 2 // Executable file
#define ET_DYN  3 // Shared object file
#define ET_CORE 4 // Core file
// elf identification, e_ident
#define EI_NIDENT 16 // Size of e_ident[]
#define EI_CLASS  4  // File class
#define EI_DATA   5  // Data encoding
// classes to be identified by e_ident[EI_CLASS]
#define ELFCLASSNONE    0 // Invalid class
#define ELFCLASS32      1 // 32-bit objects
#define ELFCLASS64      2 // 64-bit objects
// specifies the data encoding of the processor-specific data in the object file, e_ident[EI_DATA]
#define ELFDATANONE 0 // Invalid data encoding
#define ELFDATA2LSB 1 // specifies 2's complement values, with the least significant byte occupying the lowest address
#define ELFDATA2MSB 2 // specifies 2's complement values, with the most significant byte occupying the lowest address

#define SHN_UNDEF 0 // value marks an undefined, missing, irrelevant, or otherwise meaningless section reference

// elf file signature 4 bytes
#define ELFMAG0 0x7F
#define ELFMAG1 0x45 // E
#define ELFMAG2 0x4C // L
#define ELFMAG3 0x46 // F

typedef uint16_t elf32_half; 
typedef uint32_t elf32_word;
typedef int32_t elf32_sword;
typedef uint32_t elf32_addr;
typedef int32_t elf32_off;

/*
* Program Header
* An executable or shared object file's program header table is an array of structures,
* each describing a segment or other information the system needs to prepare the program for execution.
* An object file segment contains one or more sections.
* Program headers are meaningful only for executable and shared object files
* A file specifies its own program header size with the ELF header's e_phentsize and e_phnum members.
*/
struct elf32_phdr{
    elf32_word  p_type;   // kind of segment this array element describes
    elf32_off   p_offset; // the offset from the beginning of the file at which the first byte of the segment resides 
    elf32_addr  p_vaddr;  // virtual address at which the first byte of the segment resides in memory
    elf32_addr  p_paddr;  // physical address at which the first byte
    elf32_word  p_filesz; // number of bytes in the file image of the segment
    elf32_word  p_memsz;  // number of bytes in the memory image of the segment
    elf32_word  p_flags;  // flags relevant to the segment
    elf32_word  p_align;  // value to which the segments are aligned in memory and in the file, 0 and 1 mean that no alignment is required
}__attribute__((packed));

/*
* Section Header 
*/
struct elf32_shdr{
    elf32_word  sh_name;      // name of the section
    elf32_word  sh_type;      // categorizes the section's contents and semantics
    elf32_word  sh_flags;     // 1-bit flags that describe miscellaneous attributes
    elf32_addr  sh_addr;      // address at which the section's first byte should reside 
    elf32_off   sh_offset;    // byte offset from the beginning of the file to the first byte in the section
    elf32_word  sh_size;      // section's size in bytes 
    elf32_word  sh_link;      // section header table index link, whose interpretation depends on the section type
    elf32_word  sh_info;      // extra information, whose interpretation depends on the section type
    elf32_word  sh_addralign; // the value of sh_addr % sh_addralign must be 0, values 0 and 1 mean the section has no alignment constraints
    elf32_word  sh_entsize;   // size in bytes of each entry
}__attribute__((packed));

/*
* ELF Header
* Some object file control structures can grow, because the ELF header contains their actual sizes.
* If the object file format changes, a program may encounter control structures that are larger or smaller than expected.
* Programs might therefore ignore "extra" information.
* The treatment of "missing" information depends on context and will be specified when and if extensions are defined.
*/
struct elf32_header{
    unsigned char e_ident[EI_NIDENT]; // initial bytes mark the file as an object file and provide machine-independent data with which to decode and interpret the file's contents
    elf32_half  e_type; // the object file type
    elf32_half  e_machine; // required architecture for an individual file
    elf32_word  e_version; // object file version
    elf32_addr  e_entry; // virtual address to which the system first transfers control, thus starting the process
    elf32_off   e_phoff; // program header table's file offset in bytes
    elf32_off   e_shoff; // section header table's file offset in bytes
    elf32_word  e_flags; // processor-specific flags associated with the file
    elf32_half  e_ehsize; // ELF header's size in bytes
    elf32_half  e_phentsie; // size in bytes of one entry in the file's program header table; all entries are the same size.
    elf32_half  e_phnum; // number of entries in the program header table
    elf32_half  e_shentsize; // a section header's size in bytes
    elf32_half  e_shnum; // the number of entries in the section header table
    elf32_half  e_shstrndx; // section header table index of the entry associated with the section name string table
}__attribute__((packed)); 

/*
* Dynamic Section
* its program header table will have an element of type PT_DYNAMIC
* This "segment" contains the .dynamic section 
*/
struct elf32_dyn{
    elf32_sword d_tag; // controls the interpretation of d_un.
    union elf{
        elf32_word  d_val; // represents integer values with various interpretations
        elf32_addr  d_ptr; // represents program virtual addresses
    }d_un;
}__attribute__((packed));

/*
* Symbol Table
* An object file's symbol table holds information needed to locate and relocate a program's symbolic definitions and references.
* A symbol table index is a subscript into this array.
* Index 0 both designates the first entry in the table and serves as the undefined symbol index.
* The contents of the initial entry are specified later in this section.
*/
struct elf32_sym{
    elf32_word  st_name; // index into the object file's symbol string table, which holds the character representations of the symbol names.
    elf32_addr  st_value; // value of the associated symbol
    elf32_word  st_size; // number of bytes contained in the object. This member holds 0 if the symbol has no size or an unknown size.
    unsigned char   st_info; // specifies the symbol's type and binding attributes
    unsigned char   st_other; // currently holds 0 and has no defined meaning
    elf32_half  st_shndx; // Every symbol table entry is "defined" in relation to some section; this member holds the relevant section header table index
}__attribute__((packed));

void* elf_get_entry_pointer(struct elf32_header* elf_header);
uint32_t elf_get_entry(struct elf32_header* elf_header);

#endif