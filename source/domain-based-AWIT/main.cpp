#include "domain_augmented_weighted_interval_tree.hpp"


int main()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // make weight
    make_weight();

    // make queries
    make_queries();

    // build an interval tree
    domain_augmented_weighted_interval_tree DAWIT;
    DAWIT.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) DAWIT.independent_weighted_range_sampling(query_set[i]);

    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    std::cout << " Average IRS time: " << time_total_aggregate << "[microsec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";

    return 0;
}