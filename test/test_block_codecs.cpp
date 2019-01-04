#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include <vector>
#include <cstdlib>

#include "codec/block_codecs.hpp"
#include "codec/maskedvbyte.hpp"
#include "codec/streamvbyte.hpp"
#include "codec/qmx.hpp"
#include "codec/varintgb.hpp"
#include "codec/simple8b.hpp"
#include "codec/simdbp.hpp"
#include "codec/simple16.hpp"

#include "test_common.hpp"

template <typename BlockCodec>
void test_block_codec()
{
    std::vector<size_t> sizes = {1, 16, BlockCodec::block_size - 1, BlockCodec::block_size};
    for (auto size: sizes) {
        std::vector<uint32_t> values(size);
        std::generate(values.begin(), values.end(), []() { return (uint32_t)rand() % (1 << 12); });

        for (size_t tcase = 0; tcase < 2; ++tcase) {
            // test both undefined and given sum_of_values
            uint32_t sum_of_values(-1);
            if (tcase == 1) {
                sum_of_values = std::accumulate(values.begin(), values.end(), 0);
            }
            std::vector<uint8_t> encoded;
            BlockCodec::encode(values.data(), sum_of_values, values.size(), encoded);

            std::vector<uint32_t> decoded(values.size());
            uint8_t const* out = BlockCodec::decode(encoded.data(), decoded.data(),
                                                    sum_of_values, values.size());

            REQUIRE(encoded.size() == out - encoded.data());
            REQUIRE(std::equal(values.begin(), values.end(), decoded.begin()));
        }
    }
}

TEST_CASE("block_codecs")
{
    test_block_codec<ds2i::optpfor_block>();
    test_block_codec<ds2i::varint_G8IU_block>();
    test_block_codec<ds2i::streamvbyte_block>();
    test_block_codec<ds2i::maskedvbyte_block>();
    test_block_codec<ds2i::interpolative_block>();
    test_block_codec<ds2i::qmx_block>();
    test_block_codec<ds2i::varintgb_block>();
    test_block_codec<ds2i::simple8b_block>();
    test_block_codec<ds2i::simdbp_block>();
    test_block_codec<ds2i::simple16_block>();
}
