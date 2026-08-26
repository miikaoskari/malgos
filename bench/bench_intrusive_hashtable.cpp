#include "malgos/common/utils.h"
#include "malgos/intrusive/hashtable.h"
#include <benchmark/benchmark.h>
#include <vector>

namespace
{
struct userdata_t
{
    mlg_hash_node_t node;
    size_t key;
};
} // namespace

static void BM_HashtableInit(benchmark::State &state)
{
    const size_t size = state.range(0);
    std::vector<mlg_hash_head_t> buckets(size);
    mlg_hash_table_t table;

    for (auto _ : state)
    {
        mlg_hashtable_init(&table, buckets.data(), size);
        benchmark::DoNotOptimize(table);
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_HashtableInit)->Range(8, 8 << 10);

static void BM_HashtableInsert(benchmark::State &state)
{
    const size_t size = state.range(0);
    std::vector<mlg_hash_head_t> buckets(size);
    std::vector<userdata_t> data(size);
    mlg_hash_table_t table;

    for (auto _ : state)
    {
        state.PauseTiming();
        mlg_hashtable_init(&table, buckets.data(), size);
        state.ResumeTiming();

        for (size_t i = 0; i < size; i++)
        {
            mlg_hashtable_insert(&table, &data[i].node, i);
        }
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_HashtableInsert)->Range(8, 8 << 10);

static void BM_HashtableLookup(benchmark::State &state)
{
    const size_t size = state.range(0);
    std::vector<mlg_hash_head_t> buckets(size);
    std::vector<userdata_t> data(size);
    mlg_hash_table_t table;
    mlg_hashtable_init(&table, buckets.data(), size);

    for (size_t i = 0; i < size; i++)
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
        key = (key + 1) % size;
    }
    state.SetItemsProcessed(state.iterations());
}
BENCHMARK(BM_HashtableLookup)->Range(8, 8 << 10);

static void BM_HashtableRemove(benchmark::State &state)
{
    const size_t size = state.range(0);
    std::vector<mlg_hash_head_t> buckets(size);
    std::vector<userdata_t> data(size);
    mlg_hash_table_t table;

    for (auto _ : state)
    {
        state.PauseTiming();
        mlg_hashtable_init(&table, buckets.data(), size);
        for (size_t i = 0; i < size; i++)
        {
            mlg_hashtable_insert(&table, &data[i].node, i);
        }
        state.ResumeTiming();

        for (size_t i = 0; i < size; i++)
        {
            mlg_hashtable_remove(&data[i].node);
        }
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_HashtableRemove)->Range(8, 8 << 10);

static size_t rehash_key_callback(mlg_hash_node_t *nodep)
{
    userdata_t *data = mlg_container_of(nodep, userdata_t, node);
    return data->key;
}

static void BM_HashtableRehash(benchmark::State &state)
{
    const size_t size = state.range(0);
    std::vector<mlg_hash_head_t> src_buckets(size);
    std::vector<mlg_hash_head_t> dst_buckets(size);
    std::vector<userdata_t> data(size);
    mlg_hash_table_t src_table;
    mlg_hash_table_t dst_table;

    for (auto _ : state)
    {
        state.PauseTiming();
        mlg_hashtable_init(&src_table, src_buckets.data(), size);
        mlg_hashtable_init(&dst_table, dst_buckets.data(), size);
        for (size_t i = 0; i < size; i++)
        {
            data[i].key = i;
            mlg_hashtable_insert(&src_table, &data[i].node, i);
        }
        state.ResumeTiming();

        mlg_hashtable_rehash(&src_table, &dst_table, rehash_key_callback);
    }
    state.SetItemsProcessed(state.iterations() * size);
}
BENCHMARK(BM_HashtableRehash)->Range(8, 8 << 10);
