#include "../utils/utils.hpp"
#include "../utils/weighted_sampling.hpp"


double sampling_num_avg = 0;


// definition of nodes of augmented interval tree
struct node
{
    unsigned int median = 0;
    node* left_child = NULL;
    node* right_child = NULL;
    std::vector<unsigned int> left_sorted_val, right_sorted_val;
    std::vector<unsigned int> left_sorted_idx, right_sorted_idx;
    std::vector<unsigned int> augmented_left_sorted_val, augmented_right_sorted_val;
    std::vector<unsigned int> augmented_left_sorted_idx, augmented_right_sorted_idx;
};

// definition of search log
struct node_log
{
    node* n = NULL;
    unsigned int _case = 0;     // 0: left, 1: right, 2: augmented_left, 3: augmented_right
    unsigned int left_idx = 0;
    unsigned int right_idx = 0;
};

// definition of virtual interval
struct virtual_interval
{
    std::pair<unsigned int, unsigned int> v_interval;
    std::vector<unsigned int> index_vec;
};


// definition of AIT
class v_augmented_interval_tree
{
    // set of nodes
    std::vector<node> nodes;

    // set of virtual intervals
    std::vector<virtual_interval> virtual_intervals;

    // virtual interval size
    unsigned int v_size = 1;

    // random seed
    pcg32 _mt;

    /* function for making virtual intervals (sort-based) */
    void make_virtual_intervals()
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // sort
        std::sort(intervals.begin(), intervals.end());

        // get cardinality
        const unsigned int size = intervals.size();

        // determine virtual interval size
        v_size = (unsigned int)log2(size);

        unsigned int v_size_tmp = 0;
        virtual_interval _virtual_interval;
        _virtual_interval.v_interval = {domain_size, 0};

        for (unsigned int i = 0; i < size; ++i)
        {
            // increment size
            ++v_size_tmp;
          
            // store idx
            _virtual_interval.index_vec.push_back(i);
            
            // update left & right
            if (_virtual_interval.v_interval.first > intervals[i].first) _virtual_interval.v_interval.first = intervals[i].first;
            if (_virtual_interval.v_interval.second < intervals[i].second) _virtual_interval.v_interval.second = intervals[i].second;
            
            if (v_size_tmp == v_size || i == size - 1)
            {
                // store
                virtual_intervals.push_back(_virtual_interval);

                // init
                _virtual_interval.v_interval = {domain_size, 0};
                _virtual_interval.index_vec.clear();
                v_size_tmp = 0;
            }
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " virtual interval making time: " << time_preprocess << "[msec]\n";
        std::cout << " virtual_intervals.size(): " << virtual_intervals.size() << "\n";
    }

    void make_virtual_intervals_()
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        // sort
        std::sort(intervals.begin(), intervals.end());

        // get cardinality
        const unsigned int size = intervals.size();

        // determine virtual interval size
        v_size = (unsigned int)log2(size);

        std::vector<std::pair<unsigned int, unsigned int>> start_vec(size);
        for (unsigned int i = 0; i < size; ++i) start_vec[i] = {intervals[i].first, i};
        // std::sort(start_vec.begin(), start_vec.end());

        unsigned int v_size_tmp = 0;
        virtual_interval _virtual_interval;
        _virtual_interval.v_interval = {domain_size, 0};

        for (unsigned int i = 0; i < size; ++i)
        {
            // increment size
            ++v_size_tmp;

            // get idx
            const unsigned int idx = start_vec[i].second;
            
            // store idx
            _virtual_interval.index_vec.push_back(idx);
            
            // update left & right
            if (_virtual_interval.v_interval.first > intervals[idx].first) _virtual_interval.v_interval.first = intervals[idx].first;
            if (_virtual_interval.v_interval.second < intervals[idx].second) _virtual_interval.v_interval.second = intervals[idx].second;
            
            if (v_size_tmp == v_size || i == size - 1)
            {
                // store
                virtual_intervals.push_back(_virtual_interval);

                // init
                _virtual_interval.v_interval = {domain_size, 0};
                _virtual_interval.index_vec.clear();
                v_size_tmp = 0;
            }
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " virtual interval making time: " << time_preprocess << "[msec]\n";
        std::cout << " virtual_intervals.size(): " << virtual_intervals.size() << "\n";
    }

    /* function for making a node of interval tree */
    void make_node(const std::vector<unsigned int> &indexes, node* n)
    {
        // get size
        const unsigned int size = indexes.size();

        /************************/
        /* compute median value */
        /************************/

        // get 2 x size endpoints
        std::vector<unsigned int> endpoints;
        for (unsigned int i = 0 ; i < size; ++i)
        {
            // get index
            const unsigned int idx = indexes[i];

            // push
            endpoints.push_back(virtual_intervals[idx].v_interval.first);
            endpoints.push_back(virtual_intervals[idx].v_interval.second);
        }

        // sort 2 x size end points
        std::sort(endpoints.begin(), endpoints.end());

        // get median
        n->median = endpoints[size];

        /************************************/
        /* compute overlap, left, and right */
        /************************************/

        // compute overlap, left, and right
        std::vector<unsigned int> left_indexes, right_indexes;
        std::vector<std::pair<unsigned int, unsigned int>> left_sorted, right_sorted, all_left_sorted, all_right_sorted;
        for (unsigned int i = 0; i < size; ++i)
        {
            // get index
            const unsigned int idx = indexes[i];

            if (virtual_intervals[idx].v_interval.second < n->median)   // left case
            {
                left_indexes.push_back(idx);
            }
            else if (virtual_intervals[idx].v_interval.first > n->median)  // right case
            {
                right_indexes.push_back(idx);
            }
            else    // overlap case
            {
                left_sorted.push_back({virtual_intervals[idx].v_interval.first,idx});
                right_sorted.push_back({virtual_intervals[idx].v_interval.second,idx});
            }

            all_left_sorted.push_back({virtual_intervals[idx].v_interval.first,idx});
            all_right_sorted.push_back({virtual_intervals[idx].v_interval.second,idx});
        }

        // sort left_sorted & right_sorted
        std::sort(left_sorted.begin(), left_sorted.end());
        std::sort(right_sorted.begin(), right_sorted.end());
        std::sort(all_left_sorted.begin(), all_left_sorted.end());
        std::sort(all_right_sorted.begin(), all_right_sorted.end());

        // store sorted info.
        const unsigned int left_size = left_sorted.size();
        for (unsigned int i = 0; i < left_size; ++i)
        {
            n->left_sorted_val.push_back(left_sorted[i].first);     // value
            n->left_sorted_idx.push_back(left_sorted[i].second);    // idx
        }

        const unsigned int right_size = right_sorted.size();
        for (unsigned int i = 0; i < right_size; ++i)
        {
            n->right_sorted_val.push_back(right_sorted[i].first);   // value
            n->right_sorted_idx.push_back(right_sorted[i].second);  // idx
        }

        // subtree's sorted arrays
        for (unsigned int i = 0; i < size; ++i)
        {
            n->augmented_left_sorted_val.push_back(all_left_sorted[i].first);   // value
            n->augmented_left_sorted_idx.push_back(all_left_sorted[i].second);  // idx

            n->augmented_right_sorted_val.push_back(all_right_sorted[i].first);   // value
            n->augmented_right_sorted_idx.push_back(all_right_sorted[i].second);  // idx
        }        

        // get left child
        if (left_indexes.size() > 0)
        {
            node l;
            nodes.push_back(l);
            n->left_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(left_indexes, n->left_child);
        }

        // get right child
        if (right_indexes.size() > 0)
        {
            node r;
            nodes.push_back(r);
            n->right_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(right_indexes, n->right_child);
        }
    }

    /* function for computing candidates */
    void compute_candidates(const std::pair<unsigned int, unsigned int> &query, node* n, std::vector<node_log> &candidate)
    {
        if (query.second < n->median)   // range is at left of the median
        {
            // get index by binary search
            auto itr = std::upper_bound(n->left_sorted_val.begin(), n->left_sorted_val.end(), query.second);
            unsigned int distance = std::distance(n->left_sorted_val.begin(), itr);
            if (distance != 0)
            {
                --distance;
                // make a log
                node_log _log;
                _log.n = n;
                _log._case = 0;
                _log.left_idx = 0;
                _log.right_idx = distance;

                // store candidate
                candidate.push_back(_log);
            }

            // search left child
            if (n->left_child != NULL) compute_candidates(query, n->left_child, candidate);
        }
        else if (query.first > n->median)   // range is at right of the median
        {
            // get index by binary search
            auto itr = std::lower_bound(n->right_sorted_val.begin(), n->right_sorted_val.end(), query.first);
            unsigned int distance = std::distance(n->right_sorted_val.begin(), itr);
            if (distance != n->right_sorted_val.size())
            {
                // make a log
                node_log _log;
                _log.n = n;
                _log._case = 1;
                _log.right_idx = n->right_sorted_val.size() - 1;
                _log.left_idx = distance;

                // store candidate
                candidate.push_back(_log);
            }

            // search right child
            if (n->right_child != NULL) compute_candidates(query, n->right_child, candidate);
        }
        else    // query range overlaps the median
        {
            /**********************/
            /* current node check */
            /**********************/
            node_log _log;
            _log.n = n;
            _log._case = 0;
            _log.left_idx = 0;
            _log.right_idx = n->left_sorted_val.size() - 1; // all intervals overlapping the median overlaps the query
            candidate.push_back(_log);

            /*******************/
            /* left node check */
            /*******************/
            if (n->left_child != NULL)
            {
                const node* left_child = n->left_child;
                auto itr = std::lower_bound(left_child->augmented_right_sorted_val.begin(), left_child->augmented_right_sorted_val.end(), query.first);
                unsigned int distance = std::distance(left_child->augmented_right_sorted_val.begin(), itr);
                
                // if there exists overlapping intervals
                if (distance != left_child->augmented_right_sorted_val.size())
                {
                    _log.n = n->left_child;
                    _log._case = 3;
                    _log.right_idx = left_child->augmented_right_sorted_val.size() - 1;
                    _log.left_idx = distance;
                    candidate.push_back(_log);
                }
            }

            /********************/
            /* right node check */
            /********************/
            if (n->right_child != NULL)
            {
                const node* right_child = n->right_child;
                auto itr = std::upper_bound(right_child->augmented_left_sorted_val.begin(), right_child->augmented_left_sorted_val.end(), query.second);
                unsigned int distance = std::distance(right_child->augmented_left_sorted_val.begin(), itr);
                
                // if there exists overlapping intervals
                if (distance != 0)
                {
                    --distance;
                    _log.n = n->right_child;
                    _log._case = 2;
                    _log.left_idx = 0;
                    _log.right_idx = distance;
                    candidate.push_back(_log);
                }
            }
        }
    }

    /* function for get a random sample */
    unsigned int get_random_sample(const node_log *_log)
    {
        // get a random virtual interval
        const unsigned int _case = _log->_case;
        std::uniform_int_distribution<> rnd_idx(_log->left_idx, _log->right_idx);

        // index of sorted array
        const int arr_idx = rnd_idx(_mt);

        // random index
        unsigned int idx = 0;

        if (_case == 0)
        {
            idx = _log->n->left_sorted_idx[arr_idx];
        }
        else if (_case == 1)
        {
            idx = _log->n->right_sorted_idx[arr_idx];
        }
        else if (_case == 2)
        {
            idx = _log->n->augmented_left_sorted_idx[arr_idx];
        }
        else
        {
            idx = _log->n->augmented_right_sorted_idx[arr_idx];
        }

        return idx;
    }


public:

    /* constructor */
    v_augmented_interval_tree(){}

    /* function for building an interval tree */
    void build()
    {
        mem = process_mem_usage();

        /**************************/
        /* make virtual intervals */
        /**************************/
        make_virtual_intervals();

        /**************************/
        /* build an interval tree */
        /**************************/
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        const unsigned int size = virtual_intervals.size();

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
        time_preprocess += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n\n";
        std::cout << " ---- Augmented interval tree info. ----\n";
        std::cout << " #nodes: " << nodes.size() << "\n";

        mem = process_mem_usage() - mem;
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    /* function for independent range sampling */
    void independent_range_sampling(const std::pair<unsigned int, unsigned int> &query)
    {
        /*************************/
        /* candidate computation */
        /*************************/
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        std::vector<node_log> candidate;
        candidate.reserve(10);
        compute_candidates(query, &nodes[0], candidate);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_search_average += time_range_search;

        // sum result size
        result_size = candidate.size();
        result_size_aggregate += result_size;
        
        /************/
        /* sampling */
        /************/
        start = std::chrono::system_clock::now();

        if (candidate.size() > 0)
        {
            std::vector<unsigned int> samples;
            samples.reserve(sample_size);

            // building an alias structure
            std::vector<unsigned int> arr;
            for (unsigned int i = 0; i < candidate.size(); ++i) arr.push_back(candidate[i].right_idx - candidate[i].left_idx + 1);
            alias a(arr);

            // sampling a node
            while (samples.size() < sample_size)
            {
                ++sampling_num_avg;

                // get a weighted random virtual interval
                const unsigned int v_idx = get_random_sample(&candidate[a.get_indx()]);

                // get a random interval from the above virtual interval
                std::uniform_int_distribution<> rnd_idx(0, virtual_intervals[v_idx].index_vec.size() - 1);
                const unsigned int idx = virtual_intervals[v_idx].index_vec[rnd_idx(_mt)];

                // check overlap
                if (intervals[idx].second >= query.first)
                {
                    if (intervals[idx].first <= query.second) samples.push_back(idx);
                }
            }
        }

        end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_sampling_average += time_range_sampling;
        time_total_aggregate += (time_range_sampling + time_range_search);
    }
};