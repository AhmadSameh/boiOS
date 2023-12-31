#include "gdt.h"

void encode_gdt_entry(uint8_t* target, struct gdt_structured src){
    if(src.limit > 65536 && (src.limit & 0xFFF) != 0xFFF)
        panic("encoded_gdt_entry: Invalid argument");
    target[6] = 0x40;
    if(src.limit > 65536){
        src.limit = src.limit >> 12;
        target[6] = 0xC0;
    }
    // encode the limit
    target[0] = src.limit & 0xFF;
    target[1] = (src.limit >> 8) & 0xFF;
    target[6] |= (src.limit >> 16) & 0x0F;
    // encode the base
    target[2] = src.base & 0xFF;
    target[3] = (src.base >>  8) & 0xFF;
    target[4] = (src.base >> 16) & 0xFF;
    target[7] = (src.base >> 24) & 0xFF;
    // set type
    target[5] = src.type;
}

void gdt_structured_to_gdt(struct gdt* gdt, struct gdt_structured* structured_gdt, int total_entries){
    for(int i=0; i<total_entries; i++){
        // encode a single structured gdt into a normal gdt, the gdt the process can read
        encode_gdt_entry((uint8_t*)&gdt[i], structured_gdt[i]);
    }
}
