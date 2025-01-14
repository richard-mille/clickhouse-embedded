#include "config.h"

#include <gtest/gtest.h>
#include <Processors/Chunk.h>
#include <Columns/IColumn.h>
#include <Common/PODArray.h>

 namespace DB {

std::vector<ChunkOffsetsPtr> scatterOffsetsBySelector(ChunkOffsetsPtr chunk_offsets, const IColumn::Selector & selector, size_t partition_num);

class AsyncInsertsTest : public ::testing::TestPartResult
{};


TEST(AsyncInsertsTest, testScatterOffsetsBySelector)
{
    auto test_impl = [](std::vector<size_t> offsets, std::vector<size_t> selector_data, size_t part_num, std::vector<std::vector<size_t>> expected)
    {
        auto offset_ptr = std::make_shared<ChunkOffsets>(offsets);
        IColumn::Selector selector(selector_data.size());
        size_t num_rows = selector_data.size();
        for (size_t i = 0; i < num_rows; i++)
            selector[i] = selector_data[i];

        auto results = scatterOffsetsBySelector(offset_ptr, selector, part_num);
        ASSERT_EQ(results.size(), expected.size());
        for (size_t i = 0; i < results.size(); i++)
        {
            auto result = results[i]->offsets;
            auto expect = expected[i];
            ASSERT_EQ(result.size(), expect.size());
            for (size_t j = 0; j < result.size(); j++)
                ASSERT_EQ(result[j], expect[j]);
        }
    };

    test_impl({5}, {0,1,0,1,0}, 2, {{3},{2}});
    test_impl({5,10}, {0,1,0,1,0,1,0,1,0,1}, 2, {{3,5},{2,5}});
    test_impl({4,8,12}, {0,1,0,1,0,2,0,2,1,2,1,2}, 3, {{2,4},{2,4},{2,4}});
    test_impl({1,2,3,4,5}, {0,1,2,3,4}, 5, {{1},{1},{1},{1},{1}});
    test_impl({3,6,10}, {1,1,1,2,2,2,0,0,0,0}, 3, {{4},{3},{3}});
}

}
