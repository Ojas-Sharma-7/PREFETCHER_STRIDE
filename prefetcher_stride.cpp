#include "prefetcher.h"
StridePrefetcher::StridePrefetcher(unsigned window)
: window_size(window), common_stride(0) {}

void StridePrefetcher::access_memory(new_addr_type addr) {
if (!address_history.empty() && addr == address_history.back()) return;

address_history.push_back(addr);
if (address_history.size() > window_size) {
address_history.pop_front();
}

if (address_history.size() > 1) {
detect_common_stride();
generate_prefetch_addresses(addr);
}
}

void StridePrefetcher::detect_common_stride() {
stride_freq.clear();

std::deque<new_addr_type>::iterator it = address_history.begin();
new_addr_type prev = *it;
++it;

36
for (; it != address_history.end(); ++it) {
int stride = (int)(*it - prev);
prev = *it;
stride_freq[stride]++;
}
int max_count = 0;
common_stride = 0;

std::map<int, int>::iterator sit;
for (sit = stride_freq.begin(); sit != stride_freq.end(); ++sit) {
if (sit->second > max_count) {
max_count = sit->second;
common_stride = sit->first;
}
}
}
void StridePrefetcher::generate_prefetch_addresses(new_addr_type last_addr) {
prefetch_buffer.clear();
int num_prefetch = 2;

if (common_stride == 0) return;

for (int i = 1; i <= num_prefetch; ++i) {
prefetch_buffer.push_back(last_addr + i * common_stride);
}
}

std::vector<new_addr_type> StridePrefetcher::get_prefetched_addresses() {
return prefetch_buffer;
}