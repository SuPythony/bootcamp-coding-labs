#include <cstddef>
#include <iostream>
#include <utility>
#include <vector>
#include <numeric>

// This is a constant that won't change anywhere. Maybe use this to optimize something?
#define STEPS 7

struct Packet {
    int device_id;
    int lane;
    int reading;
    int quality;
    int kind;
    int stamp;
};

static unsigned int mix32(unsigned int x) {
    x ^= x >> 16;
    x *= 0x7feb352dU;
    x ^= x >> 15;
    x *= 0x846ca68bU;
    x ^= x >> 16;
    return x;
}

static std::vector<Packet> build_packets(int count, int device_count, int lane_count) {
    std::vector<Packet> packets(count);
    unsigned int seed = 123456789U;

    for (int i = 0; i < count; ++i) {
        seed = mix32(seed + i * 17U + 31U);
        Packet p;
        p.device_id = seed % device_count;
        p.lane = (seed >> 7) % lane_count;
        p.reading = (seed >> 11) & 1023;
        p.quality = (seed >> 3) & 255;
        p.kind = (seed >> 19) & 7;
        p.stamp = i;
        packets[i] = p;
    }

    return packets;
}

static std::vector<int> build_lane_weight(int lane_count) {
    std::vector<int> weight(lane_count);
    for (int i = 0; i < lane_count; ++i) {
        weight[i] = (i * 11 + 5) % 23 + 1;
    }
    return weight;
}

static std::vector<int> build_dependency_next(int n) {
    std::vector<int> next(n);
    std::vector<int> order(n);

    for (int i = 0; i < n; ++i) {
        order[i] = i;
    }

    unsigned int seed = 987654321U;
    for (int i = n - 1; i > 0; --i) {
        seed = mix32(seed + i * 29U + 11U);
        int j = seed % (i + 1);
        std::swap(order[i], order[j]);
    }

    for (int i = 0; i < n; ++i) {
        next[order[i]] = order[(i + 1) % n];
    }

    return next;
}

static std::vector<int> build_dependency_value(int n) {
    std::vector<int> value(n);
    for (int i = 0; i < n; ++i) {
        value[i] = (mix32(i * 91U + 17U) & 1023);
    }
    return value;
}

[[maybe_unused]] static std::vector<int> build_scatter_map(int n) {
    std::vector<int> remap(n);
    for (int i = 0; i < n; ++i) {
        remap[i] = mix32(i * 37U + 101U) & (n - 1);
    }
    return remap;
}

static std::vector<int> build_dependency_sums(int count, const std::vector<int>& next, const std::vector<int>& value) {
    std::vector<int> sums(count, 0);
    for (int i = 0; i < count; i++) {
        int idx = i;
        idx = next[idx];
        sums[i] += value[idx];
        idx = next[idx];
        sums[i] += value[idx];
        idx = next[idx];
        sums[i] += value[idx];
        idx = next[idx];
        sums[i] += value[idx];
        idx = next[idx];
        sums[i] += value[idx];
        idx = next[idx];
        sums[i] += value[idx];
        idx = next[idx];
        sums[i] += value[idx];
    }
    return sums;
}

static int refresh_history(std::vector<int>& history, const std::vector<Packet>& packets, int history_cols) {
    static const int rows = (int)history.size() / history_cols;
    int total = 0;

    for (const Packet& p: packets) {
        int idx = p.device_id * history_cols + (p.stamp & (history_cols - 1)); // history_cols is a power of 2 (128 or 2048)
        history[idx] = (history[idx] + p.reading + p.quality) & 2047;
    }

    for (int row_start = 0; row_start < rows * history_cols; row_start += history_cols) {
        int carry = history[row_start];
        total += (carry & 31);
        for (int c = 1; c < history_cols; ++c) {
            carry = (carry + history[row_start + c]) & 2047;
            history[row_start + c] = carry;
            total += (carry & 31);
        }
    }

    return total;
}

static int branchy_score(const Packet& p, const std::vector<int>& lane_weight) {
    int score = p.reading + lane_weight[p.lane];
    int x = p.reading ^ (p.quality << 2) ^ (p.kind * 97);

    int flag = (x & 1) > 0;
    score += flag * 19;
    score -= !flag * 7;

    flag = (x & 2) > 0;
    score ^= flag * p.quality;
    score += !flag * p.kind * 3;

    flag = (x & 4) > 0;
    score += flag * (p.reading >> 2);
    score -= !flag * (p.quality >> 3); 

    flag = (x & 8) > 0;
    score ^= flag * p.device_id;
    score += !flag * p.lane * 5;

    flag = (x & 16) > 0;
    score += flag * 31;
    score -= !flag * 11;

    flag = (x & 32) > 0;
    score ^= flag * (p.reading << 1);
    score += !flag * (p.quality & 15);

    return score & 4095;
}

static long long process_packets(
    const std::vector<Packet>& packets,
    const std::vector<int>& lane_weight,
    const std::vector<int>& dependency_sums
) {
    long long total = 0;

    for (std::size_t i = 0; i < packets.size(); ++i) {
        const Packet& p = packets[i];
        int score = branchy_score(p, lane_weight);

        int flag = ((score ^ p.quality) & 7) > 0;
        score += flag * dependency_sums[score + p.device_id];
        score += !flag * lane_weight[p.lane];

        total += score;
    }

    return total;
}

static long long run_epoch(
    std::vector<Packet>& packets,
    const std::vector<int>& lane_weight,
    const std::vector<int>& dependency_sums,
    std::vector<int>& history,
    int history_cols
) {
    long long total = refresh_history(history, packets, history_cols);

    total += process_packets(
        packets,
        lane_weight,
        dependency_sums
    );

    return total;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    const int device_count = 4096;
    const int lane_count = 32;
    const int packet_count = 220000;
    const int dependency_count = 1 << 18;
    const int history_cols = 128;   // 2048 too
    const int epochs = 6;

    std::vector<Packet> packets = build_packets(packet_count, device_count, lane_count);
    std::vector<int> lane_weight = build_lane_weight(lane_count);
    std::vector<int> dependency_next = build_dependency_next(dependency_count);
    std::vector<int> dependency_value = build_dependency_value(dependency_count);
    std::vector<int> history(device_count * history_cols, 0);

    std::vector<int> dependency_sums = build_dependency_sums(dependency_count, dependency_next, dependency_value);

    long long answer = 0;
    for (int epoch = 0; epoch < epochs; ++epoch) {
        answer += run_epoch(
            packets,
            lane_weight,
            dependency_sums,
            history,
            history_cols
        );
    }

    std::cout << (answer & 0x7fffffffffffffffLL) << '\n';
    return 0;
}
