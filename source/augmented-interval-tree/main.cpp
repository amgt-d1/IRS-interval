#include "augmented_interval_tree.hpp"


// test func. for query processing
void test_query()
{
    // parameter input
    input_parameter();

    // data input
    input_data();

    // make queries
    make_queries();


    // build an interval tree
    augmented_interval_tree AIT;
    AIT.build();

    // IRS
    for (unsigned int i = 0; i < query_size; ++i) AIT.independent_range_sampling(query_set[i]);

    // range counting
    // for (unsigned int i = 0; i < query_size; ++i) AIT.range_counting(query_set[i]);

    time_total_aggregate /= (query_size * 1000);
    time_range_search_average /= (query_size * 1000);
    time_range_sampling_average /= (query_size * 1000);
    std::cout << " Average IRS time: " << time_total_aggregate << "[microsec]\t Throughput: " << 1.0 / (time_total_aggregate / 1000000) << "[queries/sec]\n";
    
}

// test func. for insertions
void test_insertion()
{
    // parameter input
    input_parameter();

    // input data
    input_data_ins();

    // build an interval tree
    augmented_interval_tree AIT;
    AIT.build();

    // insert intervals
    for (unsigned int i = 0; i < update_count; ++i)
    {
        intervals.push_back(intervals_ins[i]);

        std::cout << " " << i + 1 << "-th ";
        AIT.insert(intervals_ins[i]);
    }

    std::cout << "\n amortized update time: " << update_time / update_count << "[msec]\n\n";
}

// test func. for batch insertions
void test_batch_insertion()
{
    // parameter input
    input_parameter();

    // input data
    input_data_ins();

    // build an interval tree
    augmented_interval_tree AIT;
    AIT.build();

    std::vector<std::pair<unsigned int, unsigned int>> insertions;
    std::vector<unsigned int> indices;

    // get batch size
    const unsigned int batch_size = log2(intervals.size()) * log2(intervals.size());
    std::cout << " batch size: " << batch_size << "\n";
    for (unsigned int i = 0; i < update_count; ++i)
    {
        intervals.push_back(intervals_ins[i]);
        insertions.push_back(intervals_ins[i]);
        indices.push_back(intervals.size() - 1);

        bool f = 0;
        if (insertions.size() == batch_size)
        {
            f = 1;
        }
        else if (i == update_count - 1)
        {
            f = 1;
        }

        if (f)
        {
            std::cout << " " << i + 1 << "-th ";
            AIT.batch_insert(insertions, indices);

            // init
            insertions.clear();
            indices.clear();
        }
    }

    std::cout << "\n amortized update time: " << update_time / update_count << "[msec]\n\n";
}

// test func. for deletion
void test_deletion()
{
    // parameter input
    input_parameter();

    // input data
    input_data();

    // get del intervals
    make_deletion();

    // build an interval tree
    augmented_interval_tree AIT;
    AIT.build();

    // delete intervals
    for (unsigned int i = 0; i < update_count; ++i)
    {
        std::cout << " " << i + 1 << "-th ";
        AIT.deletion(indices_del[i]);
    }

    std::cout << "\n amortized update time: " << update_time / update_count << "[msec]\n\n";
}


int main()
{    
    // test query processing
    test_query();

    // test insertion
    // test_insertion();
    // test_batch_insertion();

    // test deletion
    // test_deletion();

    return 0;
}