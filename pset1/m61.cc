#define M61_DISABLE 1
#include "m61.hh"
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <cinttypes>
#include <cassert>
#include <unordered_map>

/// m61_malloc(sz, file, line)
///    Return a pointer to `sz` bytes of newly-allocated dynamic memory.
///    The memory is not initialized. If `sz == 0`, then m61_malloc must
///    return a unique, newly-allocated pointer value. The allocation
///    request was at location `file`:`line`.

struct m61_statistics tmp = {0, 0, 0, 0, 0, 0, 0, 0};
static std::unordered_map<uintptr_t, size_t> local_allocs;

//re-adjust heap_max and heap_min after freeing
void* m61_malloc(size_t sz, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    
    // Your code here.
    if((int)sz < 0)
    {
        ++tmp.ntotal;
        ++tmp.nfail;
        tmp.fail_size += sz;
        return NULL;
    }
    
    
    void* ptr = base_malloc(sz);
    if(ptr)
    {
        if(disabled)
        {
            ++tmp.ntotal;
            ++tmp.nactive;
            tmp.total_size += (unsigned long long)sz;
            tmp.active_size += (unsigned long long)sz;
            if(reinterpret_cast<uintptr_t>(ptr) + sz > tmp.heap_max)
            {
                tmp.heap_max = reinterpret_cast<uintptr_t>(ptr) + sz;
            }

            if(reinterpret_cast<uintptr_t>(ptr) < tmp.heap_min)
            {
                tmp.heap_min = reinterpret_cast<uintptr_t>(ptr);
            }

            local_allocs[reinterpret_cast<uintptr_t>(ptr)] = sz;
        }
    }

    if(ptr == NULL)
    {
        ++tmp.ntotal;
        ++tmp.nfail;
    }
    
    return ptr;
}


/// m61_free(ptr, file, line)
///    Free the memory space pointed to by `ptr`, which must have been
///    returned by a previous call to m61_malloc. If `ptr == NULL`,
///    does nothing. The free was called at location `file`:`line`.

void m61_free(void* ptr, const char* file, long line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings
    // Your code here.
    //check for tmp.nactive > 0 or not
    if(!ptr)
    {
        return;
    }             
    std::unordered_map<uintptr_t, size_t>::iterator iter = local_allocs.find(reinterpret_cast<uintptr_t>(ptr));
    if(iter != local_allocs.end())
    {
        tmp.active_size -= iter->second;
        tmp.nactive -= 1;   
        local_allocs.erase(iter);
        return;
    }
    base_free(ptr); 
    tmp.nactive -= 1;
}


/// m61_calloc(nmemb, sz, file, line)
///    Return a pointer to newly-allocated dynamic memory big enough to
///    hold an array of `nmemb` elements of `sz` bytes each. If `sz == 0`,
///    then must return a unique, newly-allocated pointer value. Returned
///    memory should be initialized to zero. The allocation request was at
///    location `file`:`line`.

void* m61_calloc(size_t nmemb, size_t sz, const char* file, long line) {
    // Your code here (to fix test019).
    if((int)sz < 0 || (int)nmemb < 0)
    {
        return NULL;
    }
    
    void* ptr = m61_malloc(nmemb * sz, file, line);
    if (ptr) {
        memset(ptr, 0, nmemb * sz);
    }
    return ptr;
}


/// m61_get_statistics(stats)
///    Store the current memory statistics in `*stats`.

void m61_get_statistics(m61_statistics* stats) {

    // Stub: set all statistics to enormous numbers : 
    //memset(stats, 0, sizeof(m61_statistics)); test 1
    *stats = tmp;
}


/// m61_print_statistics()
///    Print the current memory statistics.

void m61_print_statistics() {
    m61_statistics stats;
    m61_get_statistics(&stats);

    printf("alloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("alloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}


/// m61_print_leak_report()
///    Print a report of all currently-active allocated blocks of dynamic
///    memory.

void m61_print_leak_report() {
    // Your code here.
}


/// m61_print_heavy_hitter_report()
///    Print a report of heavily-used allocation locations.

void m61_print_heavy_hitter_report() {
    // Your heavy-hitters code here
}
