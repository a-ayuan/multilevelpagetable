#include "mlpt.h"
#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define SQUARED (1 << POBITS) 
#define PAGE_ENTRY_COUNT (1 << (POBITS - 3)) 
#define VAL_BIT 0x1 
#define TRANSLATION_FAILURE ~0

size_t ptbr = 0;

// Helper function to extract the index for the current page table level
size_t extract_index(size_t vpn, int level) {
    return (vpn >> ((POBITS - 3) * (LEVELS - 1 - level))) & (PAGE_ENTRY_COUNT - 1);
}

// Helper function to follow a page table entry at a given index
size_t* follow_pagetable_entry(size_t* p_table, size_t idx, size_t pobits_offset) {
    if (p_table == NULL || (p_table[idx] & VAL_BIT) == 0) {
        return NULL;
    }
    return (size_t*)(p_table[idx] & ~pobits_offset);
}

size_t translate(size_t va) {
    if (ptbr == 0) {
        return TRANSLATION_FAILURE;
    }

    size_t pobits_offset = SQUARED - 1;
    size_t vpn = va >> POBITS;
    size_t offset = va & pobits_offset;
    size_t* p_table = (size_t*)ptbr;

    for (int level = 0; level < LEVELS; ++level) {
        size_t idx = extract_index(vpn, level);
        p_table = follow_pagetable_entry(p_table, idx, pobits_offset);
        if (p_table == NULL) {
            return TRANSLATION_FAILURE;
        }
    }

    return ((size_t)p_table & ~pobits_offset) | offset;
}

void page_allocate(size_t va) {
    if (ptbr == 0) {
        void* root_page;
        posix_memalign(&root_page, SQUARED, SQUARED);
        ptbr = (size_t) root_page;
        memset((void*)ptbr, 0, PAGE_ENTRY_COUNT * sizeof(size_t));
    }

    size_t mask = SQUARED - 1;
    size_t vpn = va >> POBITS;
    size_t* table = (size_t*) ptbr;

    for (int level = 0; level < LEVELS - 1; level++) {
        size_t index = (vpn >> ((LEVELS - 1 - level) * (POBITS - 3))) & (PAGE_ENTRY_COUNT - 1);

        if (table[index] == 0) {
            void* new_table;
            posix_memalign(&new_table, SQUARED, SQUARED);
            table[index] = (size_t)new_table | VAL_BIT;
            memset(new_table, 0, PAGE_ENTRY_COUNT * sizeof(size_t));
        }

        table = (size_t*)(table[index] & ~mask);
    }

    size_t final_index = vpn & (PAGE_ENTRY_COUNT - 1);
    if (table[final_index] == 0) {
        void* page;
        posix_memalign(&page, SQUARED, SQUARED);
        table[final_index] = (size_t)page | VAL_BIT;
        memset(page, 0, PAGE_ENTRY_COUNT * sizeof(size_t));
    }
}

//Added deallocation method
void page_deallocate(size_t va) {
    if (ptbr == 0) {
        return;
    }

    size_t pobits_offset = SQUARED - 1;
    size_t vpn = va >> POBITS;
    size_t* p_table = (size_t*)ptbr;
    size_t* tables_to_free[LEVELS];
    size_t indices[LEVELS];
    int levels_filled = 0;

    for (int level = 0; level < LEVELS; ++level) {
        size_t idx = extract_index(vpn, level);
        tables_to_free[level] = p_table;
        indices[level] = idx;
        levels_filled++;

        if ((p_table[idx] & VAL_BIT) == 0) {
            return;
        }

        if (level == LEVELS - 1) {
            break;
        }

        p_table = (size_t*)(p_table[idx] & ~pobits_offset);
    }

    size_t phys_addr = p_table[indices[LEVELS - 1]] & ~pobits_offset;
    free((void*)phys_addr);

    p_table[indices[LEVELS - 1]] = 0;

    for (int level = LEVELS - 1; level >= 0; --level) {
        size_t* table = tables_to_free[level];
        bool is_empty = true;

        for (size_t i = 0; i < PAGE_ENTRY_COUNT; ++i) {
            if (table[i] != 0) {
                is_empty = false;
                break;
            }
        }

        if (is_empty) {
            free(table);
            if (level > 0) {
                tables_to_free[level - 1][indices[level - 1]] = 0;
            } else {
                ptbr = 0;
            }
        } else {
            break;
        }
    }
}

int main() {
    // 0 pages have been allocated
    assert(ptbr == 0);

    page_allocate(0x456789abcdef);
    // 5 pages have been allocated: 4 page tables and 1 data
    assert(ptbr != 0);

    page_allocate(0x456789abcd00);
    // no new pages allocated (still 5)
    
    int *p1 = (int *)translate(0x456789abcd00);
    *p1 = 0xaabbccdd;
    short *p2 = (short *)translate(0x456789abcd02);
    printf("%04hx\n", *p2); // prints "aabb\n"

    assert(translate(0x456789ab0000) == 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x456789ab0000);
    // 1 new page allocated (now 6; 4 page table, 2 data)

    assert(translate(0x456789ab0000) != 0xFFFFFFFFFFFFFFFF);
    
    page_allocate(0x456780000000);
    // 2 new pages allocated (now 8; 5 page table, 3 data)
}
