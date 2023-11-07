#include "kds_weight.hpp"


int main()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // get weights
    make_weight();

    // make queries
    make_queries();

    // Weighted IRS
    kds_weight KDS;
    for (unsigned int i = 0; i < query_size; ++i) KDS.independent_range_sampling(query_set[i]);

    // aggregate total result
    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    sampling_num_avg /= query_size;
    std::cout << " Average IRS time: " << time_total_aggregate / 1000 << "[millisec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\t Avg. #samplings: " << sampling_num_avg << "\n\n";


    return 0;
}