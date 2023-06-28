#include "../utils/utils.hpp"
#include "../utils/weighted_sampling.hpp"


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

    unsigned int flag = 0;  // update flag
};

// definition of search log
struct node_log
{
    node* n = NULL;
    unsigned int _case = 0;     // 0: left, 1: right, 2: augmented_left, 3: augmented_right
    unsigned int left_idx = 0;
    unsigned int right_idx = 0;
};


// definition of AIT
class augmented_interval_tree
{
    // set of nodes
    std::vector<node> nodes;

    // random seed
    pcg32 _mt;

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
            endpoints.push_back(intervals[idx].first);
            endpoints.push_back(intervals[idx].second);
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
                left_sorted.push_back({intervals[idx].first,idx});
                right_sorted.push_back({intervals[idx].second,idx});
            }

            all_left_sorted.push_back({intervals[idx].first,idx});
            all_right_sorted.push_back({intervals[idx].second,idx});
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
        if (n != &nodes[0])
        {
            for (unsigned int i = 0; i < size; ++i)
            {
                n->augmented_left_sorted_val.push_back(all_left_sorted[i].first);   // value
                n->augmented_left_sorted_idx.push_back(all_left_sorted[i].second);  // idx

                n->augmented_right_sorted_val.push_back(all_right_sorted[i].first);   // value
                n->augmented_right_sorted_idx.push_back(all_right_sorted[i].second);  // idx
            }
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

    /* function for recursive insertion */
    void recursive_insertion(const std::pair<unsigned int, unsigned int> &ins, node* n)
    {
        // update augmented lists
        const unsigned int size = n->augmented_left_sorted_idx.size();

        // get idx
        const unsigned int idx = intervals.size() - 1;

        std::vector<std::pair<unsigned int, unsigned int>> all_left_sorted, all_right_sorted;
        for (unsigned int i = 0; i < size; ++i)
        {
            all_left_sorted.push_back({n->augmented_left_sorted_val[i], n->augmented_left_sorted_idx[i]});
            all_right_sorted.push_back({n->augmented_right_sorted_val[i], n->augmented_right_sorted_idx[i]});
        }
        all_left_sorted.push_back({ins.first, idx});
        all_right_sorted.push_back({ins.second, idx});

        std::sort(all_left_sorted.begin(), all_left_sorted.end());
        std::sort(all_right_sorted.begin(), all_right_sorted.end());

        // clear
        n->augmented_left_sorted_val.clear();
        n->augmented_left_sorted_idx.clear();
        n->augmented_right_sorted_val.clear();
        n->augmented_right_sorted_idx.clear();

        // subtree's sorted arrays
        for (unsigned int i = 0; i < size + 1; ++i)
        {
            n->augmented_left_sorted_val.push_back(all_left_sorted[i].first);   // value
            n->augmented_left_sorted_idx.push_back(all_left_sorted[i].second);  // idx

            n->augmented_right_sorted_val.push_back(all_right_sorted[i].first);   // value
            n->augmented_right_sorted_idx.push_back(all_right_sorted[i].second);  // idx
        }

        // check overlap
        if (ins.second < n->median)   // left case
        {
            if (n->left_child != NULL)
            {
                recursive_insertion(ins, n->left_child);
            }
            else
            {
                node l;
                nodes.push_back(l);
                n->left_child = &nodes[nodes.size() - 1];

                // make a sub-tree
                std::vector<unsigned int> left_indexes;
                left_indexes.push_back(idx);
                make_node(left_indexes, n->left_child);
            }
        }
        else if (ins.first > n->median)  // right case
        {
            if (n->right_child != NULL)
            {
                recursive_insertion(ins, n->right_child);
            }
            else
            {
                node l;
                nodes.push_back(l);
                n->right_child = &nodes[nodes.size() - 1];

                // make a sub-tree
                std::vector<unsigned int> right_indexes;
                right_indexes.push_back(idx);
                make_node(right_indexes, n->right_child);
            }
        }
        else 
        {
            // update sorted lists
            const unsigned int _size = n->left_sorted_idx.size();

            std::vector<std::pair<unsigned int, unsigned int>> left_sorted, right_sorted;
            for (unsigned int i = 0; i < _size; ++i)
            {
                left_sorted.push_back({n->left_sorted_val[i], n->left_sorted_idx[i]});
                right_sorted.push_back({n->right_sorted_val[i], n->right_sorted_idx[i]});
            }
            left_sorted.push_back({ins.first, idx});
            right_sorted.push_back({ins.second, idx});

            std::sort(left_sorted.begin(), left_sorted.end());
            std::sort(right_sorted.begin(), right_sorted.end());

            // clear
            n->left_sorted_val.clear();
            n->left_sorted_idx.clear();
            n->right_sorted_val.clear();
            n->right_sorted_idx.clear();

            for (unsigned int i = 0; i < _size + 1; ++i)
            {
                n->left_sorted_val.push_back(left_sorted[i].first);     // value
                n->left_sorted_idx.push_back(left_sorted[i].second);    // idx
                n->right_sorted_val.push_back(right_sorted[i].first);     // value
                n->right_sorted_idx.push_back(right_sorted[i].second);    // idx
            }
        }
    }

    /* function for recursive insertion (batch mode) */
    void recursive_batch_insertion(const std::pair<unsigned int, unsigned int> &ins, const unsigned int &idx, node* n)
    {
        // update flag
        n->flag = 1;

        // update augmented lists
        n->augmented_left_sorted_val.push_back(ins.first);
        n->augmented_left_sorted_idx.push_back(idx);
        n->augmented_right_sorted_val.push_back(ins.second);
        n->augmented_right_sorted_idx.push_back(idx);

        // check overlap
        if (ins.second < n->median)   // left case
        {
            if (n->left_child != NULL)
            {
                recursive_batch_insertion(ins, idx, n->left_child);
            }
            else
            {
                node l;
                nodes.push_back(l);
                n->left_child = &nodes[nodes.size() - 1];

                // make a sub-tree
                std::vector<unsigned int> left_indexes;
                left_indexes.push_back(idx);
                make_node(left_indexes, n->left_child);
            }
        }
        else if (ins.first > n->median)  // right case
        {
            if (n->right_child != NULL)
            {
                recursive_batch_insertion(ins, idx, n->right_child);
            }
            else
            {
                node l;
                nodes.push_back(l);
                n->right_child = &nodes[nodes.size() - 1];

                // make a sub-tree
                std::vector<unsigned int> right_indexes;
                right_indexes.push_back(idx);
                make_node(right_indexes, n->right_child);
            }
        }
        else 
        {
            // update flag
            n->flag = 2;

            // update sorted lists
            n->left_sorted_val.push_back(ins.first);
            n->left_sorted_idx.push_back(idx);
            n->right_sorted_val.push_back(ins.second);
            n->right_sorted_idx.push_back(idx);
        }
    }

    /* function for recursive deletion */
    void recursive_deletion(const unsigned int &idx, node* n)
    {
        /*************************/
        /* augmented list update */
        /*************************/
        const unsigned int size = n->augmented_right_sorted_idx.size();

        // deletion from augmented list (left)
        for (unsigned int i = 0; i < size; ++i)
        {
            if (n->augmented_left_sorted_idx[i] == idx)
            {
                n->augmented_left_sorted_idx.erase(n->augmented_left_sorted_idx.begin() + i);
                n->augmented_left_sorted_val.erase(n->augmented_left_sorted_val.begin() + i);
                break;
            }
        }

        // deletion from augmented list (right)
        for (unsigned int i = 0; i < size; ++i)
        {
            if (n->augmented_right_sorted_idx[i] == idx)
            {
                n->augmented_right_sorted_idx.erase(n->augmented_right_sorted_idx.begin() + i);
                n->augmented_right_sorted_val.erase(n->augmented_right_sorted_val.begin() + i);
                break;
            }
        }

        // check overlap
        if (intervals[idx].second < n->median)   // left case
        {
            if (n->left_child != NULL) recursive_deletion(idx, n->left_child);
        }
        else if (intervals[idx].first > n->median)  // right case
        {
            if (n->right_child != NULL) recursive_deletion(idx, n->right_child);
        }
        else 
        {
            // deletion from sorted list (left)
            const unsigned int _size = n->left_sorted_idx.size();
            for (unsigned int i = 0; i < _size; ++i)
            {
                if (n->left_sorted_idx[i] == idx)
                {
                    n->left_sorted_idx.erase(n->left_sorted_idx.begin() + i);
                    n->left_sorted_val.erase(n->left_sorted_val.begin() + i);
                    break;
                }
            }

            // deletion from sorted list (right)
            for (unsigned int i = 0; i < _size; ++i)
            {
                if (n->right_sorted_idx[i] == idx)
                {
                    n->right_sorted_idx.erase(n->right_sorted_idx.begin() + i);
                    n->right_sorted_val.erase(n->right_sorted_val.begin() + i);
                    break;
                }
            }
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
    augmented_interval_tree(){}

    /* function for building an interval tree */
    void build()
    {
        mem = process_mem_usage();

        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        /**************************/
        /* build an interval tree */
        /**************************/
        const unsigned int size = intervals.size();

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
        std::cout << " ---- Augmented interval tree info. ----\n";
        std::cout << " #nodes: " << nodes.size() << "\n";
        // std::cout << " height: " << height << "\n";

        mem = process_mem_usage() - mem;
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    /* function for inserting an interval */
    void insert(const std::pair<unsigned int, unsigned int> &ins)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        recursive_insertion(ins, &nodes[0]);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " insertion time: " << time << "[msec]\n";
        update_time += time;
    }

    /* function for batch insertion */
    void batch_insert(const std::vector<std::pair<unsigned int, unsigned int>> &ins, const std::vector<unsigned int> &indices)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        const unsigned int size = ins.size();
        for (unsigned int i = 0; i < size; ++i) recursive_batch_insertion(ins[i], indices[i], &nodes[0]);

        // update each node
        const unsigned int node_size = nodes.size();
        for (unsigned int i = 0; i < node_size; ++i)
        {
            if (nodes[i].flag > 0)
            {
                node* n = &nodes[i];

                // update augmented lists
                unsigned int _size = n->augmented_left_sorted_idx.size();

                std::vector<std::pair<unsigned int, unsigned int>> all_left_sorted, all_right_sorted;
                for (unsigned int i = 0; i < _size; ++i)
                {
                    all_left_sorted.push_back({n->augmented_left_sorted_val[i], n->augmented_left_sorted_idx[i]});
                    all_right_sorted.push_back({n->augmented_right_sorted_val[i], n->augmented_right_sorted_idx[i]});
                }

                std::sort(all_left_sorted.begin(), all_left_sorted.end());
                std::sort(all_right_sorted.begin(), all_right_sorted.end());

                // clear
                n->augmented_left_sorted_val.clear();
                n->augmented_left_sorted_idx.clear();
                n->augmented_right_sorted_val.clear();
                n->augmented_right_sorted_idx.clear();

                // subtree's sorted arrays
                for (unsigned int i = 0; i < _size + 1; ++i)
                {
                    n->augmented_left_sorted_val.push_back(all_left_sorted[i].first);   // value
                    n->augmented_left_sorted_idx.push_back(all_left_sorted[i].second);  // idx

                    n->augmented_right_sorted_val.push_back(all_right_sorted[i].first);   // value
                    n->augmented_right_sorted_idx.push_back(all_right_sorted[i].second);  // idx
                }

                if (nodes[i].flag == 2)
                {
                    // update sorted lists
                    _size = n->left_sorted_idx.size();

                    std::vector<std::pair<unsigned int, unsigned int>> left_sorted, right_sorted;
                    for (unsigned int i = 0; i < _size; ++i)
                    {
                        left_sorted.push_back({n->left_sorted_val[i], n->left_sorted_idx[i]});
                        right_sorted.push_back({n->right_sorted_val[i], n->right_sorted_idx[i]});
                    }

                    std::sort(left_sorted.begin(), left_sorted.end());
                    std::sort(right_sorted.begin(), right_sorted.end());

                    // clear
                    n->left_sorted_val.clear();
                    n->left_sorted_idx.clear();
                    n->right_sorted_val.clear();
                    n->right_sorted_idx.clear();

                    for (unsigned int i = 0; i < _size + 1; ++i)
                    {
                        n->left_sorted_val.push_back(left_sorted[i].first);     // value
                        n->left_sorted_idx.push_back(left_sorted[i].second);    // idx
                        n->right_sorted_val.push_back(right_sorted[i].first);   // value
                        n->right_sorted_idx.push_back(right_sorted[i].second);  // idx
                    }
                }

                // init update flag
                n->flag = 0;
            }
        }

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " insertion time: " << time << "[msec]\n";
        update_time += time;
    }

    /* function for deleting an interval */
    void deletion(const unsigned int &idx)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        recursive_deletion(idx, &nodes[0]);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " deletion time: " << time << "[msec]\n";
        update_time += time;
    }

    /* function for independent range sampling */
    void independent_range_sampling(const std::pair<unsigned int, unsigned int> &query)
    {
        /*************************/
        /* candidate computation */
        /*************************/
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        std::vector<node_log> candidate;
        candidate.reserve(40);
        compute_candidates(query, &nodes[0], candidate);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_search_average += time_range_search;
      
        // sum result size
        result_size = candidate.size();
        result_size_aggregate += result_size;
        // std::cout << " range search time: " << time_range_search << "[microsec]\t result size: " << result_size << "\n";

        /************/
        /* sampling */
        /************/
        start = std::chrono::system_clock::now();

        if (candidate.size() > 0)
        {
            std::vector<unsigned int> samples(sample_size);

            // building an alias structure
            std::vector<unsigned int> arr;
            for (unsigned int i = 0; i < candidate.size(); ++i) arr.push_back(candidate[i].right_idx - candidate[i].left_idx + 1);
            alias a(arr);

            // sampling a node
            for (unsigned int i = 0; i < sample_size; ++i) samples[i] = get_random_sample(&candidate[a.get_indx()]);
        }

        end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        time_range_sampling_average += time_range_sampling;
        // std::cout << " IRS time: " << time_range_sampling << "[microsec]\n";
        // std::cout << " Total time: " << time_range_sampling + time_range_search << "[microsec]\n\n";
        time_total_aggregate += (time_range_sampling + time_range_search);
    }

    /* function for range counting */
    void range_counting(const std::pair<unsigned int, unsigned int> &query)
    {
        /*************************/
        /* candidate computation */
        /*************************/
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        std::vector<node_log> candidate;
        candidate.reserve(40);
        compute_candidates(query, &nodes[0], candidate);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        time_range_search_average += time_range_search;
        time_total_aggregate += time_range_search;
      
        // sum result size
        result_size = 0;
        for (unsigned int i = 0; i < candidate.size(); ++i) result_size += (candidate[i].right_idx - candidate[i].left_idx + 1);
        result_size_aggregate += result_size;
        // std::cout << " range search time: " << time_range_search << "[microsec]\t result size: " << result_size << "\n";
    }
};