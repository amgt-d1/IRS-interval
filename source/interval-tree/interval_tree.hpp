#include "../utils/utils.hpp"
#include "../utils/weighted_sampling.hpp"


struct node
{
    unsigned int median = 0;
    node* left_child = NULL;
    node* right_child = NULL;
    std::vector<std::pair<unsigned int, unsigned int>> left_sorted;
    std::vector<std::pair<unsigned int, unsigned int>> right_sorted;
    unsigned int height = 0;
};


class interval_tree
{
    /* member variable */
    std::vector<unsigned int> sorted_array_val, sorted_array_idx;   // for 1st phase range search
    std::vector<node> nodes;
    unsigned int height = 0;

    /* function for making a node of interval tree */
    void make_node(const std::vector<unsigned int> &indexes, node* n)
    {
        // get size
        const unsigned int size = indexes.size();

        // get 2 x size endpoints
        std::vector<unsigned int> endpoints;
        for (unsigned int i = 0 ; i < size; ++i)
        {
            // get index
            const unsigned int idx = indexes[i];

            // push
            endpoints.push_back(intervals[idx].first);
            endpoints.push_back(intervals[idx].second);
        }

        // sort 2 x size end points
        std::sort(endpoints.begin(), endpoints.end());

        // get median
        n->median = endpoints[size];

        // compute overlap, left, and right
        std::vector<unsigned int> left_indexes, right_indexes;
        for (unsigned int i = 0; i < size; ++i)
        {
            // get index
            const unsigned int idx = indexes[i];

            if (intervals[idx].second < n->median)   // left case
            {
                left_indexes.push_back(idx);
            }
            else if (intervals[idx].first > n->median)  // right case
            {
                right_indexes.push_back(idx);
            }
            else    // overlap case
            {
                n->left_sorted.push_back({intervals[idx].first,idx});
                n->right_sorted.push_back({intervals[idx].second,idx});
            }
        }

        // sort left_sorted & right_sorted
        std::sort(n->left_sorted.begin(), n->left_sorted.end());        // ascending
        std::sort(n->right_sorted.rbegin(), n->right_sorted.rend());    // descending

        // get left child
        if (left_indexes.size() > 0)
        {
            node l;
            l.height = n->height + 1;
            if (l.height > height) height = l.height;

            nodes.push_back(l);
            n->left_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(left_indexes, n->left_child);
        }

        // get right child
        if (right_indexes.size() > 0)
        {
            node r;
            r.height = n->height + 1;
            if (r.height > height) height = r.height;

            nodes.push_back(r);
            n->right_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(right_indexes, n->right_child);
        }
    }

    /* function for stabbing */
    void stabbing(node* n, const unsigned int query, std::vector<bool> &result)
    {
        if (n->median > query)  // visit left child case
        {
            // access left-sorted
            for (unsigned int i = 0; i < n->left_sorted.size(); ++i)
            {
                if (n->left_sorted[i].first <= query)
                {
                    // result.insert(n->left_sorted[i].second);
                    result[n->left_sorted[i].second] = 1; 
                }
                else
                {
                    break;
                }
            }

            // go to left child
            if (n->left_child != NULL) stabbing(n->left_child, query, result);
        }
        else if (n->median <= query)    // visit right child case
        {
            // access right-sorted
            for (unsigned int i = 0; i < n->right_sorted.size(); ++i)
            {
                if (n->right_sorted[i].first >= query)
                {
                    // result.insert(n->right_sorted[i].second);
                    result[n->right_sorted[i].second] = 1;
                }
                else
                {
                    break;
                }
            }

            // go to right child
            if (n->right_child != NULL) stabbing(n->right_child, query, result);
        }
    }

    /* function for range search */
    std::vector<unsigned int> range_search(const std::pair<unsigned int, unsigned int> &query)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // get size
        const unsigned int size = sorted_array_val.size();

        // prepare result
        std::vector<bool> result(size);

        /***********************/
        /* binary search phase */
        /***********************/

        // get position
        auto itr = std::lower_bound(sorted_array_val.begin(), sorted_array_val.end(), query.first);
        const unsigned int pos = std::distance(sorted_array_val.begin(), itr);

        // access intervals within the query range
        for (unsigned int i = pos; i < size; ++i)
        {
            if (sorted_array_val[i] <= query.second)
            {
                result[sorted_array_idx[i]] = 1;
            }
            else
            {
                break;
            }
        }

        /******************/
        /* stabbing phase */
        /******************/
        stabbing(&nodes[0], query.first, result);

        // get idx matching the query range
        std::vector<unsigned int> range_search_result;
        for (unsigned int i = 0; i < size; ++i)
        {
            if (result[i]) range_search_result.push_back(i);
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_search_average += time_range_search;
        
        // sum result size
        result_size = range_search_result.size();
        result_size_aggregate += range_search_result.size();
        // std::cout << " range search time: " << time_range_search << "[microsec]\t result size: " << result_size << "\n";

        return range_search_result;
    }


public:

    /* constructor */
    interval_tree(){}

    /* function for building an interval tree */
    void build()
    {
        mem = process_mem_usage();

        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        const unsigned int size = intervals.size();

        /***********************/
        /* make a sorted array */
        /***********************/
        std::vector<std::pair<unsigned int, unsigned int>> sorted_array;
        for (unsigned int i = 0; i < size; ++i)
        {
            sorted_array.push_back({intervals[i].first,i});     // insert <left, ID> 
            sorted_array.push_back({intervals[i].second,i});    // insert <right, ID>
        }
        std::sort(sorted_array.begin(), sorted_array.end());
        for (unsigned int i = 0; i < sorted_array.size(); ++i)
        {
            sorted_array_val.push_back(sorted_array[i].first);
            sorted_array_idx.push_back(sorted_array[i].second);
        }

        /**************************/
        /* build an interval tree */
        /**************************/

        // get indexes of all intervals
        std::vector<unsigned int> indexes;
        for (unsigned int i = 0; i < size; ++i) indexes.push_back(i);

        // reserve memory
        nodes.reserve(size);

        // make a root node
        node n;
        nodes.push_back(n);

        // recursive partition
        make_node(indexes, &nodes[0]);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n\n";
        std::cout << " ---- Interval tree info. ----\n";
        std::cout << " #nodes: " << nodes.size() << "\n";
        std::cout << " height: " << height << "\n";

        mem = process_mem_usage() - mem;
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    /* function for independent range sampling */
    void independent_range_sampling(const std::pair<unsigned int, unsigned int> &query)
    {
        // range search
        std::vector<unsigned int> range_search_result = range_search(query);

        // sampling
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        if (range_search_result.size() > 0)
        {
            std::vector<unsigned int> samples(sample_size);
            std::mt19937 mt;
            std::uniform_int_distribution<> rnd_idx(0, range_search_result.size() - 1);
            for (unsigned int i = 0; i < sample_size; ++i) samples[i] = range_search_result[rnd_idx(mt)];
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_sampling_average += time_range_sampling;
        // std::cout << " IRS time: " << time_range_sampling << "[microsec]\n";
        // std::cout << " Total time: " << time_range_sampling + time_range_search << "[microsec]\n\n";
        time_total_aggregate += (time_range_sampling + time_range_search);
    }

    /* function for weighted independent range sampling */
    void weighted_independent_range_sampling(const std::pair<unsigned int, unsigned int> &query)
    {
        // range search
        std::vector<unsigned int> range_search_result = range_search(query);

        // sampling
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        const unsigned int range_search_result_size = range_search_result.size();
        if (range_search_result_size > 0)
        {
            std::vector<unsigned int> samples(sample_size);
            std::mt19937 mt;

            // building an alias structure
            std::vector<unsigned int> arr;
            for (unsigned int i = 0; i < range_search_result_size; ++i) arr.push_back(weights[range_search_result[i]]);
            alias a(arr);
            
            // sampling a node
            for (unsigned int i = 0; i < sample_size; ++i) samples[i] = range_search_result[a.get_indx()];
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_sampling_average += time_range_sampling;
        time_total_aggregate += (time_range_sampling + time_range_search);
    }
};