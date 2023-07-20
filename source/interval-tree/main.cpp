#include "interval_tree.hpp"


// non-weighted interval case
void test_uniform()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // make queries
    make_queries();


    // build an interval tree
    interval_tree I;
    I.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) I.independent_range_sampling(query_set[i]);

    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    std::cout << " Average IRS time: " << time_total_aggregate / 1000 << "[millisec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";

    result_size_aggregate /= query_size;
    selectivity = result_size_aggregate / intervals.size();
    std::cout << " Average result size: " << result_size_aggregate << "\n";
    std::cout << " Average selectivity: " << selectivity << "\n\n"; 
}

// weighted interval case
void test_weighted()
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
    interval_tree I;
    I.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) I.weighted_independent_range_sampling(query_set[i]);

    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    std::cout << " Average IRS time: " << time_total_aggregate / 1000 << "[millisec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";

    result_size_aggregate /= query_size;
    selectivity = result_size_aggregate / intervals.size();
    std::cout << " Average result size: " << result_size_aggregate << "\n";
    std::cout << " Average selectivity: " << selectivity << "\n\n";
}


int main()
{
    // uniform case
    test_uniform();

    // weighted case
    // test_weighted();

    return 0;
}
