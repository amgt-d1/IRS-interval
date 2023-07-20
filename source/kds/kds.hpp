#include "../utils/utils.hpp"
#include "../utils/weighted_sampling.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>


double sampling_num_avg = 0;


// leaf size definition
const unsigned int leaf_size = 256;

// definition of 2d point
struct point
{    
    unsigned int id = 0;
    unsigned int x = 0;
    unsigned int y = 0;    
};

// definition of nodes of kd-tree
struct Node
{
    point location;    
    Node *leftChild = NULL;
    Node *rightChild = NULL;
    
    unsigned int depth = 0;
    unsigned int subtree_size = 0;
    
    unsigned int leftmost = 0;
    unsigned int rightmost = 0;
    unsigned int upmost = 0;
    unsigned int downmost = 0;    
};


int comparex(const void *a, const void *b) { return  ((point *)a)->x - ((point *)b)->x; }
int comparey(const void *a, const void *b) { return  ((point *)a)->y - ((point *)b)->y; }
int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int min(const int a, const int b)
{
    if (a > b) return b;
    return a;
}

int max(const int a, const int b)
{
    if (a < b) return b;
    return a;
}


class kd_tree
{
private:

    // root node of kd-tree
    Node* root = NULL;

    // random generator
    std::mt19937 mt;

    /* member functions */
    Node* partition(point *pointlist, int right, unsigned int depth)
    {    
        if (right < 0)
        {
            return NULL;
        }
        
        if (right == 0)
        {            
            Node *new_node = (Node*)malloc(sizeof(Node));            
            new_node->location = pointlist[right];            
            new_node->leftChild = NULL;
            new_node->rightChild = NULL;            
            new_node->depth = depth;            
            new_node->leftmost = new_node->location.x;
            new_node->rightmost = new_node->location.x;
            new_node->upmost = new_node->location.y;
            new_node->downmost = new_node->location.y;
            new_node->subtree_size = right + 1;
            
            return new_node;            
        }
        
        unsigned int axis = depth % 2;
        
        Node *new_node = (Node*)malloc(sizeof(Node));
        
        if (axis == 0)
        {
            qsort(pointlist, right + 1, sizeof(point), comparex);   // 要素数 = right + 1;
            new_node->leftmost = pointlist[0].x;
            new_node->rightmost = pointlist[right].x;
        }
        else if (axis == 1)
        {
            qsort(pointlist, right + 1, sizeof(point), comparey);
            new_node->downmost = pointlist[0].y;
            new_node->upmost = pointlist[right].y;
        }
        
        unsigned int median = (right) / 2;
        
        new_node->location = pointlist[median];
        new_node->depth = depth;        
        new_node->rightChild = partition(pointlist + median + 1, right - (median + 1), depth + 1);
        new_node->leftChild = partition(pointlist, median, depth + 1);
        new_node->subtree_size = right + 1;
        
        if (axis == 0)
        {            
            if (new_node->rightChild != NULL && new_node->leftChild != NULL)
            {
                new_node->upmost = max(new_node->rightChild->upmost, new_node->leftChild->upmost);
                new_node->downmost = min(new_node->rightChild->downmost, new_node->leftChild->downmost);                
            }
            else if (new_node->rightChild != NULL)
            {
                new_node->upmost = new_node->rightChild->upmost;
                new_node->downmost = new_node->rightChild->downmost;                
            }
            else if (new_node->leftChild != NULL)
            {
                new_node->upmost = new_node->leftChild->upmost;
                new_node->downmost = new_node->leftChild->downmost;
            }
            else
            {
                new_node->upmost = new_node->location.y;
                new_node->downmost = new_node->location.y;
            }            
        }
        else
        {            
            if (new_node->rightChild != NULL && new_node->leftChild != NULL)
            {
                new_node->rightmost = max(new_node->rightChild->rightmost, new_node->leftChild->rightmost);
                new_node->leftmost = min(new_node->rightChild->leftmost, new_node->leftChild->leftmost);                
            }
            else if (new_node->rightChild != NULL)
            {
                new_node->rightmost = new_node->rightChild->rightmost;
                new_node->leftmost = new_node->rightChild->leftmost;                
            }
            else if (new_node->leftChild != NULL)
            {
                new_node->rightmost = new_node->leftChild->rightmost;
                new_node->leftmost = new_node->leftChild->leftmost;
            }
            else
            {
                new_node->rightmost = new_node->location.x;
                new_node->leftmost = new_node->location.x;
            }            
        }
        
        return new_node;        
    }

    // check fully covered
    bool is_contained(Node *region, unsigned int sx, unsigned int tx, unsigned int sy, unsigned int ty)
    {        
        if (region->leftmost < sx || region->rightmost> tx || region->upmost > ty || region->downmost < sy) return 0;        
        return 1;
    }

    // access to fully covered nodes
    void report_subtree(Node *region, std::vector<Node*> &result)
    {     
        // range sampling
        result.push_back(region);
        
        // here is range reporting area
        // if (region->leftChild == NULL && (region->rightChild == NULL))
        // {
        //     result.push_back(region);
        //     return;
        // }
        
        if (region->leftChild != NULL) report_subtree(region->leftChild, result);
        if (region->rightChild != NULL) report_subtree(region->rightChild, result);
        
        return;
    }

    // traverse nodes overlapping query MBR
    void traverse(Node *v, const unsigned int sx, const unsigned int tx, const unsigned int sy, const unsigned int ty, std::vector<Node*> &result)
    {
        if (v->rightmost < sx || v->leftmost > tx || v->downmost > ty || v->upmost < sy) return;

        if (v->subtree_size <= leaf_size)
        {
            // range sampling
            result.push_back(v);
            return;
        }
        else
        {
            if (v->leftChild == NULL && v->rightChild == NULL)  // leaf node
            {
                // range sampling
                result.push_back(v);
                return;
            }

            if (v->leftChild != NULL)
            {            
                traverse(v->leftChild, sx, tx, sy, ty, result);
            }        
            if (v->rightChild != NULL)
            {            
                traverse(v->rightChild, sx, tx, sy, ty, result);
            }
        }

        return;
    }
    
    // orthogonal range search
    void orthogonal_range_search(Node *v, const unsigned int sx, const unsigned int tx, const unsigned int sy, const unsigned int ty, std::vector<Node*> &result)
    {       
        if (v->rightmost < sx || v->leftmost > tx || v->downmost > ty || v->upmost < sy) return;        
        if (v->leftChild == NULL && v->rightChild == NULL)  // leaf node
        {
            // range sampling
            result.push_back(v);

            // here is range reporting area
            // if (sx <= v->location.x && sy <= v->location.y && tx >= v->location.x && ty >= v->location.y)
            // {
            //     result.push_back(v);
            // }
            return;
        }
        
        if (v->leftChild != NULL)
        {            
            if (is_contained(v->leftChild, sx, tx, sy, ty))
            {
                report_subtree(v->leftChild, result);
            }
            else
            {
                orthogonal_range_search(v->leftChild, sx, tx, sy, ty, result);
            }
        }        
        if (v->rightChild != NULL)
        {            
            if (is_contained(v->rightChild, sx, tx, sy, ty))
            {
                report_subtree(v->rightChild, result);
            }
            else
            {
                orthogonal_range_search(v->rightChild, sx, tx, sy, ty, result);
            }
        }

        return;
    }

    // orthogonal range counting
    void orthogonal_range_counting(Node *v, const unsigned int sx, const unsigned int tx, const unsigned int sy, const unsigned int ty, unsigned int &_result_size)
    {       
        if (v->rightmost < sx || v->leftmost > tx || v->downmost > ty || v->upmost < sy) return;

        if (sx <= v->location.x && sy <= v->location.y && tx >= v->location.x && ty >= v->location.y) ++_result_size;
        
        // leaf node
        if (v->leftChild == NULL && v->rightChild == NULL)  return;
        
        if (v->leftChild != NULL)
        {            
            if (is_contained(v->leftChild, sx, tx, sy, ty))
            {
                _result_size += (v->subtree_size - 1);
            }
            else
            {
                orthogonal_range_counting(v->leftChild, sx, tx, sy, ty, _result_size);
            }
        }        
        if (v->rightChild != NULL)
        {            
            if (is_contained(v->rightChild, sx, tx, sy, ty))
            {
                _result_size += (v->subtree_size - 1);
            }
            else
            {
                orthogonal_range_counting(v->rightChild, sx, tx, sy, ty, _result_size);
            }
        }

        return;
    }

    // traverse a sample from v
    Node* traverse_to_sample(Node *v, int &hop_count)
    {
        Node* s = NULL;
        if (v->leftChild == NULL && v->rightChild == NULL)  // leaf node
        {
            return v;
        }

        if (hop_count == 0)
        {
            return v;
        }
        else
        {
            // decrement #hops
            --hop_count;

            // random traverse
            if (v->leftChild != NULL && v->rightChild != NULL)
            {
                std::uniform_int_distribution<> rnd(0, 1);
                if (rnd(mt) <= 0.5)
                {
                    s = traverse_to_sample(v->leftChild, hop_count);
                }
                else
                {
                    s = traverse_to_sample(v->rightChild, hop_count);
                }
            }
            else if (v->leftChild != NULL)
            {
                s = traverse_to_sample(v->leftChild, hop_count);
            }
            else
            {
                s = traverse_to_sample(v->rightChild, hop_count);
            }
        }

        return s;
    }

public:

    // constructor
    kd_tree(){}

    // build a kd-tree
    void build(point *pointlist, const unsigned int size)
    {
        root = partition(pointlist, size, 0);
    }

    // IRS
    void independent_range_sampling(const std::pair<unsigned int, unsigned int>& interval)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        std::vector<Node*> result;
        // orthogonal_range_search(root, 0, interval.second, interval.first, UINT_MAX, result);
        traverse(root, 0, interval.second, interval.first, UINT_MAX, result);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_search_average += time_range_search;
        
        result_size = result.size();
        // std::cout << " range search time: " << time_range_search << "[microsec]\t candidate size: " << result_size << "\n";

        // sampling
        start = std::chrono::system_clock::now();

        if (result_size > 0)
        {
            std::vector<unsigned int> samples;
            samples.reserve(sample_size);

            // compute 2^x where x = hop-count
            unsigned int hop_count = 0;
            unsigned int leaf_s = leaf_size;
            while (leaf_s > 1)
            {
                leaf_s /= 2;
                ++hop_count;
            }

            // std::uniform_int_distribution<> rnd_idx(0, result_size - 1);
            // while (samples.size() < sample_size)
            // {
            //     Node* v = result[rnd_idx(mt)];
            //     if (0 <= v->location.x && interval.first <= v->location.y && interval.second >= v->location.x && UINT_MAX >= v->location.y) samples.push_back(v->location.id);
            // }

            // build an alias
            std::vector<unsigned int> arr;
            for (unsigned int i = 0; i < result_size; ++i) arr.push_back(result[i]->subtree_size);
            alias a(arr);

            // sampling a node
            std::uniform_int_distribution<> rnd_hop(0, hop_count);
            while (samples.size() < sample_size)
            {
                ++sampling_num_avg;
                
                // weighted sampling
                Node* v = result[a.get_indx()];
                
                // get random hop in [0, hop_count]
                int hop = rnd_hop(mt);

                // get a sample
                v = traverse_to_sample(v, hop);

                // check within the query
                if (0 <= v->location.x && interval.first <= v->location.y && interval.second >= v->location.x && UINT_MAX >= v->location.y) samples.push_back(v->location.id);
            }
        }

        end = std::chrono::system_clock::now();
        time_range_sampling = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_sampling_average += time_range_sampling;
        // std::cout << " IRS time: " << time_range_sampling << "[microsec]\n";
        
        time_total_aggregate += (time_range_sampling + time_range_search);

        // output query performance
        output_result(1);
    }    

    // range counting
    void range_counting(const std::pair<unsigned int, unsigned int>& interval)
    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();

        unsigned int _result_size = 0;
        orthogonal_range_counting(root, 0, interval.second, interval.first, UINT_MAX, _result_size);

        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        time_range_search = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() / 1000;
        time_range_search_average += time_range_search;
        time_total_aggregate += time_range_search;

        result_size = _result_size;

        // output query performance
        output_result(1);
    }
};