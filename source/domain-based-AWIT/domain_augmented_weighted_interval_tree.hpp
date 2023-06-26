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
    std::vector<unsigned int> left_sorted_weight, right_sorted_weight;
    std::vector<unsigned int> augmented_left_sorted_val, augmented_right_sorted_val;
    std::vector<unsigned int> augmented_left_sorted_idx, augmented_right_sorted_idx;
    std::vector<unsigned int> augmented_left_sorted_weight, augmented_right_sorted_weight;
};

// definition of search log
struct node_log
{
    node* n = NULL;
    unsigned int _case = 0;     // 0: left, 1: right, 2: augmented_left, 3: augmented_right
    unsigned int left_idx = 0;
    unsigned int right_idx = 0;
    unsigned int weight = 0;
};


//definition of DAWIT
class domain_augmented_weighted_interval_tree
{
    // set of nodes
    std::vector<node> nodes;

    // random seed
    pcg32 _mt;

    /* function for making a node of interval tree */
    void make_node(std::vector<unsigned int> &indexes, node* n, const unsigned int domain_left, const unsigned int domain_right)
    {
        // get size
        const unsigned int size = indexes.size();

        /************************/
        /* compute median value */
        /************************/
        n->median = domain_right + domain_left;
        n->median /= 2;

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
        unsigned int cumulative_weight = 0;
        for (unsigned int i = 0; i < left_size; ++i)
        {
            n->left_sorted_val.push_back(left_sorted[i].first);     // value
            n->left_sorted_idx.push_back(left_sorted[i].second);    // idx

            cumulative_weight += weights[left_sorted[i].second];
            n->left_sorted_weight.push_back(cumulative_weight);     // weight
        }

        const unsigned int right_size = right_sorted.size();
        cumulative_weight = 0;
        for (unsigned int i = 0; i < right_size; ++i)
        {
            n->right_sorted_val.push_back(right_sorted[i].first);   // value
            n->right_sorted_idx.push_back(right_sorted[i].second);  // idx

            cumulative_weight += weights[right_sorted[i].second];
            n->right_sorted_weight.push_back(cumulative_weight);     // weight
        }

        // subtree's sorted arrays
        cumulative_weight = 0;
        for (unsigned int i = 0; i < size; ++i)
        {
            n->augmented_left_sorted_val.push_back(all_left_sorted[i].first);   // value
            n->augmented_left_sorted_idx.push_back(all_left_sorted[i].second);  // idx

            cumulative_weight += weights[all_left_sorted[i].second];
            n->augmented_left_sorted_weight.push_back(cumulative_weight);       // weight
        }
        cumulative_weight = 0;
        for (unsigned int i = 0; i < size; ++i)
        {
            n->augmented_right_sorted_val.push_back(all_right_sorted[i].first);   // value
            n->augmented_right_sorted_idx.push_back(all_right_sorted[i].second);  // idx

            cumulative_weight += weights[all_right_sorted[i].second];
            n->augmented_right_sorted_weight.push_back(cumulative_weight);          // weight
        }        

        // get left child
        if (left_indexes.size() > 0)
        {
            node l;
            nodes.push_back(l);
            n->left_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(left_indexes, n->left_child, domain_left, n->median - 1);
        }

        // get right child
        if (right_indexes.size() > 0)
        {
            node r;
            nodes.push_back(r);
            n->right_child = &nodes[nodes.size() - 1];

            // make a sub-tree
            make_node(right_indexes, n->right_child, n->median + 1, domain_right);
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
                _log.weight = n->left_sorted_weight[distance];

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
                _log.weight = n->right_sorted_weight[_log.right_idx];
                if (distance > 0) _log.weight -= n->right_sorted_weight[distance - 1];

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
            _log.weight = n->left_sorted_weight[_log.right_idx];

            // store candidate
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
                    _log.weight = left_child->augmented_right_sorted_weight[_log.right_idx];
                    if (distance > 0) _log.weight -= left_child->augmented_right_sorted_weight[distance - 1];
                    
                    // store candidate
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
                    _log.weight = right_child->augmented_left_sorted_weight[distance];

                    // store candidate
                    candidate.push_back(_log);
                }
            }
        }
    }

    /* function for get a weighted random sample */
    unsigned int get_weighted_random_sample(node_log *_log)
    {
        const unsigned int _case = _log->_case;

        // random index
        unsigned int idx = 0;

        if (_case == 0) // left
        {
            std::uniform_int_distribution<> rnd_weight(_log->n->left_sorted_weight[_log->left_idx], _log->n->left_sorted_weight[_log->right_idx]);
            auto itr = std::lower_bound(_log->n->left_sorted_weight.begin(), _log->n->left_sorted_weight.end(), rnd_weight(_mt));
            idx = _log->n->left_sorted_idx[std::distance(_log->n->left_sorted_weight.begin(), itr)];
        }
        else if (_case == 1)    // right
        {
            std::uniform_int_distribution<> rnd_weight(_log->n->right_sorted_weight[_log->left_idx], _log->n->right_sorted_weight[_log->right_idx]);
            auto itr = std::lower_bound(_log->n->right_sorted_weight.begin(), _log->n->right_sorted_weight.end(), rnd_weight(_mt));
            idx = _log->n->right_sorted_idx[std::distance(_log->n->right_sorted_weight.begin(), itr)];
        }
        else if (_case == 2)    // augmented_left
        {
            std::uniform_int_distribution<> rnd_weight(_log->n->augmented_left_sorted_weight[_log->left_idx], _log->n->augmented_left_sorted_weight[_log->right_idx]);
            auto itr = std::lower_bound(_log->n->augmented_left_sorted_weight.begin(), _log->n->augmented_left_sorted_weight.end(), rnd_weight(_mt));
            idx = _log->n->augmented_left_sorted_idx[std::distance(_log->n->augmented_left_sorted_weight.begin(), itr)];
        }
        else    // augmented_right
        {
            std::uniform_int_distribution<> rnd_weight(_log->n->augmented_right_sorted_weight[_log->left_idx], _log->n->augmented_right_sorted_weight[_log->right_idx]);
            auto itr = std::lower_bound(_log->n->augmented_right_sorted_weight.begin(), _log->n->augmented_right_sorted_weight.end(), rnd_weight(_mt));
            idx = _log->n->augmented_right_sorted_idx[std::distance(_log->n->augmented_right_sorted_weight.begin(), itr)];
        }

        return idx;
    }

public:

    /* constructor */
    domain_augmented_weighted_interval_tree(){}

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

        // compute domain left & right
        const unsigned int domain_left = 0;
        const unsigned int domain_right = (unsigned int)std::pow(2.0, domain_exponent);

        // recursive partition
        make_node(indexes, &nodes[0], domain_left, domain_right);

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

    /* function for independent range sampling */
    void independent_weighted_range_sampling(std::pair<unsigned int, unsigned int> &query)
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
            for (unsigned int i = 0; i < candidate.size(); ++i) arr.push_back(candidate[i].weight);
            alias a(arr);

            // sampling a node
            for (unsigned int i = 0; i < sample_size; ++i) samples[i] = get_weighted_random_sample(&candidate[a.get_indx()]);
        }

        end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_sampling_average += time_range_sampling;
        // std::cout << " IRS time: " << time_range_sampling << "[microsec]\n";
        // std::cout << " Total time: " << time_range_sampling + time_range_search << "[microsec]\n\n";
        time_total_aggregate += (time_range_sampling + time_range_search);
    }
};
