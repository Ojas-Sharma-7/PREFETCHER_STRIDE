
-#ifndef PREFETCHER_H_
#define PREFETCHER_H_

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <iostream>
using namespace std;

class StridePrefetcher {
public:
    StridePrefetcher(unsigned int num_prefetch = 2) : m_num_prefetch(num_prefetch) {}

    vector<uint64_t> access(unsigned warp_id, uint64_t addr) {
        vector<uint64_t> prefetches;
        if (m_last_addr.find(warp_id) != m_last_addr.end()) {
            int64_t stride = addr - m_last_addr[warp_id];
            if (m_last_stride.find(warp_id) != m_last_stride.end()) {
                if (stride == m_last_stride[warp_id]) {
                    uint64_t next_addr = addr;
                    for (unsigned int i = 1; i <= m_num_prefetch; i++) {
                        next_addr += stride;
                        prefetches.push_back(next_addr);
                    }
                }
            }
            m_last_stride[warp_id] = stride;
        }
        m_last_addr[warp_id] = addr;
        return prefetches;
    }

    void debug_print() const {
        cout << "----Stride Prefetcher Table----" << endl;
        for (auto &entry : m_last_addr) {
            unsigned warp_id = entry.first;
            cout << "Warp " << warp_id << " last_addr=0x" << hex << entry.second
                 << " stride=" << dec << m_last_stride.at(warp_id) << endl;
        }
        cout << "--------------------------------" << endl;
    }

private:
    unsigned int m_num_prefetch;
    unordered_map<unsigned, uint64_t> m_last_addr;
    unordered_map<unsigned, int64_t> m_last_stride;
};

#endif
