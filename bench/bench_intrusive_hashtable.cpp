#include "malgos/common/utils.h"
#include "malgos/intrusive/hashtable.h"
#include <benchmark/benchmark.h>
#include <cstddef>
#include <vector>

namespace
{
struct userdata_t
{
    mlg_hash_node_t node;
    size_t key;
};

constexpr size_t kSize = 4096;
} // namespace

static void BM_HashtableInit(benchmark::State &state)
{
    const size_t size = static_cast<size_t>(state.range(0));
    std::vector<mlg_hash_head_t> buckets(size);
    mlg_hash_table_t table;

    for (auto _ : state)
    {
        mlg_hashtable_init(&table, buckets.data(), size);
        benchmark::DoNotOptimize(table);
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_HashtableInit)->RangeMultiplier(2)->Range(8, 8 << 10);

static void BM_HashtableInsert(benchmark::State &state)
{
    std::vector<mlg_hash_head_t> buckets(kSize);
    std::vector<userdata_t> data(kSize);
    mlg_hash_table_t table;

    for (auto _ : state)
    {
        state.PauseTiming();
        mlg_hashtable_init(&table, buckets.data(), kSize);
        state.ResumeTiming();

        for (size_t i = 0; i < kSize; i++)
        {
            mlg_hashtable_insert(&table, &data[i].node, i);
        }
    }
    state.SetItemsProcessed(state.iterations() * kSize);
}
BENCHMARK(BM_HashtableInsert);

static void BM_HashtableLookupChain(benchmark::State &state)
{
    constexpr size_t kBuckets = 64;
    const size_t items = static_cast<size_t>(state.range(0));

    std::vector<mlg_hash_head_t> buckets(kBuckets);
    std::vector<userdata_t> data(items);
    mlg_hash_table_t table;
    mlg_hashtable_init(&table, buckets.data(), kBuckets);

    for (size_t i = 0; i < items; i++)
    {
        data[i].key = i;
        mlg_hashtable_insert(&table, &data[i].node, i);
    }

    size_t key = 0;
    for (auto _ : state)
    {
        mlg_hash_node_t *pos;
        mlg_hash_for_each_possible(pos, &table, key)
        {
            benchmark::DoNotOptimize(pos);
        }
        key = (key + 1) % kBuckets;
    }
    state.SetItemsProcessed(state.iterations());
    state.counters["chain_len"] = static_cast<double>(items) / static_cast<double>(kBuckets);
}
BENCHMARK(BM_HashtableLookupChain)->RangeMultiplier(4)->Range(64, 64 << 10);

static void BM_HashtableRemove(benchmark::State &state)
{
    std::vector<mlg_hash_head_t> buckets(kSize);
    std::vector<userdata_t> data(kSize);
    mlg_hash_table_t table;

    for (auto _ : state)
    {
        state.PauseTiming();
        mlg_hashtable_init(&table, buckets.data(), kSize);
        for (size_t i = 0; i < kSize; i++)
        {
            mlg_hashtable_insert(&table, &data[i].node, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < kSize; i++)
        {
            mlg_hashtable_remove(&data[i].node);
        }
    }
    state.SetItemsProcessed(state.iterations() * kSize);
}
BENCHMARK(BM_HashtableRemove);

static size_t rehash_key_callback(mlg_hash_node_t *nodep)
{
    userdata_t *data = mlg_container_of(nodep, userdata_t, node);
    return data->key;
}

static void BM_HashtableRehash(benchmark::State &state)
{
    std::vector<mlg_hash_head_t> src_buckets(kSize);
    std::vector<mlg_hash_head_t> dst_buckets(kSize);
    std::vector<userdata_t> data(kSize);
    mlg_hash_table_t src_table;
    mlg_hash_table_t dst_table;

    for (auto _ : state)
    {
        state.PauseTiming();
        mlg_hashtable_init(&src_table, src_buckets.data(), kSize);
        mlg_hashtable_init(&dst_table, dst_buckets.data(), kSize);
        for (size_t i = 0; i < kSize; i++)
        {
            data[i].key = i;
            mlg_hashtable_insert(&src_table, &data[i].node, i);
        }
        state.ResumeTiming();

        mlg_hashtable_rehash(&src_table, &dst_table, rehash_key_callback);
    }
    state.SetItemsProcessed(state.iterations() * kSize);
}
BENCHMARK(BM_HashtableRehash);
