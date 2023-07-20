#include "kds.hpp"


int main()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // make queries
    make_queries();

    // convert interval into 2d point
    const unsigned int size = intervals.size();
    point* datapoints = new point[size];
    for (unsigned int i = 0; i < size; ++i)
    {
        datapoints[i].x = intervals[i].first;
        datapoints[i].y = intervals[i].second;
        datapoints[i].id = i;
    }

    // make an instance
    kd_tree kdtree;

    // get memory before building a kd-tree
    mem = process_mem_usage();

    // start pre-processing
    std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

    kdtree.build(datapoints, size - 1);

    std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
    time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
    std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n";
    
    mem = process_mem_usage() - mem;
    std::cout << " Memory: " << mem << "[MB]\n";
    std::cout << " -----------------------------\n\n";

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) kdtree.independent_range_sampling(query_set[i]);

    // range counting
    // for (unsigned int i = 0; i < query_size; ++i) kdtree.range_counting(query_set[i]);

    // aggregate total result
    time_total_aggregate /= query_size;
    time_range_search_average /= query_size;
    time_range_sampling_average /= query_size;
    std::cout << " Average IQS time: " << time_total_aggregate / 1000 << "[millisec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n\n";

    return 0;
}