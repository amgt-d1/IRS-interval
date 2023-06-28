#include "domain_augmented_interval_tree.hpp"


int main()
{
    // parameter input
    input_parameter();

    // data input
    input_data();
    // output_data();

    // make queries
    make_queries();


    // build an interval tree
    domain_augmented_interval_tree DAIT;
    DAIT.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) DAIT.independent_range_sampling(query_set[i]);

    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    std::cout << " Average IRS time: " << time_total_aggregate << "[microsec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";

    return 0;
}