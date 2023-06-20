#include "v_augmented_interval_tree.hpp"


int main()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // make queries
    make_queries();

    // make virtual intervals & build an AIT
    v_augmented_interval_tree AIT;
    AIT.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) AIT.independent_range_sampling(query_set[i]);

    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    std::cout << " Average IRS time: " << time_total_aggregate << "[microsec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";

    sampling_num_avg /= query_size;
    std::cout << " Average #sampling: " << sampling_num_avg << "\n\n";

    return 0;
}