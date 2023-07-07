#include "augmented_weighted_interval_tree.hpp"


// test func. for query processing
void test_query()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // get weights
    make_weight();

    // make queries
    make_queries();

    // build an augmented weighted interval tree
    augmented_weighted_interval_tree AWIT;
    AWIT.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) AWIT.independent_weighted_range_sampling(query_set[i]);

    time_total_aggregate /= (query_size * 1000);
    time_range_search_average /= (query_size * 1000);
    time_range_sampling_average /= (query_size * 1000);
    std::cout << " Average IRS time: " << time_total_aggregate << "[microsec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";
}


int main()
{    
    // test query processing
    test_query();

    return 0;
}
