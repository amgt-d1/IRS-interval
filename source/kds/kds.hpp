#include "../utils/utils.hpp"
#include "../utils/weighted_sampling.hpp"
#include "kdtree.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>


double sampling_num_avg = 0;


class kds
{
    point* datapoints = NULL;

    // root node of kd-tree
    Node* root = NULL;

    // random generator
    std::mt19937 mt;


public:

    // constructor
    kds()
    {
        // prepare new format
        const unsigned int size = intervals.size();
        datapoints = new point[size];
        for (unsigned int i = 0; i < size; ++i)
        {
            datapoints[i].x = intervals[i].first;
            datapoints[i].y = intervals[i].second;
            datapoints[i].idx = i;
        }

        // kd-tree building
        mem = process_mem_usage();
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        root = kdtree(datapoints, size - 1, 0, 0, size - 1);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_preprocess = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() / 1000;
        std::cout << " Pre-processing time: " << time_preprocess << "[msec]\n";

        mem = process_mem_usage() - mem;
        std::cout << " Memory: " << mem << "[MB]\n";
        std::cout << " -----------------------------\n\n";
    }

    // independent range sampling
    void independent_range_sampling(const std::pair<unsigned int, unsigned int>& interval)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        
        std::vector<std::pair<Node*, bool>> result;
        SearchKDtree(root, result, 0, interval.second, interval.first, UINT_MAX);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_search_average += time_range_search;
        result_size = result.size();
        // std::cout << " range search time: " << time_range_search << "[microsec]\t candidate size: " << result_size << "\t";

        // sampling
        start = std::chrono::system_clock::now();

        unsigned int sampling_num = 0;
        if (result.size() > 0)
        {
            std::vector<unsigned int> samples;
            samples.reserve(sample_size);

            // build an alias structure
            std::vector<unsigned int> arr;
            for (unsigned int i = 0; i < result.size(); ++i)
            {
                if (result[i].second == 0)
                {
                    arr.push_back(1);
                }
                else
                {
                    // arr.push_back(result[i].first->subtree_size);
                    arr.push_back(result[i].first->right_idx - result[i].first->left_idx + 1);
                }
            }
            alias b(arr);

            // sampling
            while (samples.size() < sample_size)
            {
                ++sampling_num;

                const unsigned int idx = b.get_indx();
                Node* v = result[idx].first;

                if (result[idx].second == 1)
                {
                    while (1)
                    {
                        if (v->leftChild == NULL && v->rightChild == NULL)
                        {
                            if (0 <= v->location.x && interval.first <= v->location.y && interval.second >= v->location.x && UINT_MAX >= v->location.y) samples.push_back(v->location.idx);
                            break;
                        }

                        if (v->subtree_size <= capacity)
                        {
                            std::uniform_int_distribution<> rnd_idx(v->left_idx, v->right_idx);
                            const unsigned int _idx = rnd_idx(mt);
                            if (0 <= datapoints[_idx].x && interval.first <= datapoints[_idx].y && interval.second >= datapoints[_idx].x && UINT_MAX >= datapoints[_idx].y) samples.push_back(datapoints[_idx].idx);
                            break;
                        }

                        arr.clear();
                        arr.push_back(1);
                        if (v->leftChild != NULL) arr.push_back(v->leftChild->subtree_size);
                        if (v->rightChild != NULL) arr.push_back(v->rightChild->subtree_size);
                        alias c(arr);
                        const unsigned int idx = c.get_indx();
                        if (idx == 0)
                        {
                            if (0 <= v->location.x && interval.first <= v->location.y && interval.second >= v->location.x && UINT_MAX >= v->location.y) samples.push_back(v->location.idx);
                            break;
                        }
                        if (idx == 1) 
                        {
                            if (v->leftChild != NULL) 
                            {
                                v = v->leftChild;
                            }
                            else
                            {
                                v = v->rightChild;
                            }
                        }
                        if (idx == 2) v = v->rightChild;
                    }
                }
                else
                {
                    if (0 <= v->location.x && interval.first <= v->location.y && interval.second >= v->location.x && UINT_MAX >= v->location.y) samples.push_back(v->location.idx);
                }

            }
        }

        end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_sampling_average += time_range_sampling;
        // std::cout << " IRS time: " << time_range_sampling << "[microsec]\t" << std::cout << " #iterations: " << sampling_num << "\n";
        sampling_num_avg += sampling_num;
        
        time_total_aggregate += (time_range_sampling + time_range_search);
    }

};
