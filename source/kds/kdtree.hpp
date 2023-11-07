#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>


const unsigned int capacity = 8192;


// definition of point (geo-spatial)
struct point
{
    unsigned int idx = 0;
    unsigned int x = 0;
    unsigned int y = 0;
};

// definition of kd-tree node
struct Node
{    
    point location;    
    Node *leftChild = NULL;
    Node *rightChild = NULL;    
    unsigned int subtree_size = 0;

    unsigned int left_idx = 0;
    unsigned int right_idx = 0;
    
    unsigned int leftmost = 0;
    unsigned int rightmost = 0;
    unsigned int upmost = 0;
    unsigned int downmost = 0;    
};


int comparex(const void *a, const void *b) { return  ((point *)a)->x - ((point *)b)->x; }
int comparey(const void *a, const void *b) { return  ((point *)a)->y - ((point *)b)->y; }
int compare(const void *a, const void *b) { return *(int *)a - *(int *)b; }

int min(const unsigned int a, const unsigned int b)
{
    if (a > b) return b;
    return a;
}

int max(const unsigned int a, const unsigned int b)
{
    if (a < b) return b;
    return a;
}


// creat a node of kd-tree
Node *kdtree(point *pointlist, int right, int depth)
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
        new_node->subtree_size = right + 1;
        new_node->leftmost = new_node->location.x;
        new_node->rightmost = new_node->location.x;
        new_node->upmost = new_node->location.y;
        new_node->downmost = new_node->location.y;
        
        return new_node;        
    }
    
    int axis = depth % 2;
    
    Node *new_node = (Node*)malloc(sizeof(Node));
    
    if (axis == 0)
    {
        qsort(pointlist, right + 1, sizeof(point), comparex);
        new_node->leftmost = pointlist[0].x;
        new_node->rightmost = pointlist[right].x;
    }
    else if (axis == 1)
    {
        qsort(pointlist, right + 1, sizeof(point), comparey);
        new_node->downmost = pointlist[0].y;
        new_node->upmost = pointlist[right].y;
    }
    
    int median = right / 2;
    
    new_node->location = pointlist[median];
    new_node->subtree_size = right + 1;    
    new_node->rightChild = kdtree(pointlist + median + 1, right - (median + 1), depth + 1);
    new_node->leftChild = kdtree(pointlist, median, depth + 1);
    
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
            new_node->downmost =  new_node->location.y;
        }        
    }
    else
    {        
        if (new_node->rightChild != NULL && new_node->leftChild != NULL)
        {
            new_node->rightmost = max(new_node->rightChild->rightmost,new_node->leftChild->rightmost);
            new_node->leftmost = min(new_node->rightChild->leftmost,new_node->leftChild->leftmost);
            
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
            new_node->leftmost =  new_node->location.x;
        }        
    }
    
    return new_node;    
}

Node *kdtree(point *pointlist, int right, int depth, const unsigned int left_idx, const unsigned int right_idx)
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
        new_node->subtree_size = right + 1;
        new_node->leftmost = new_node->location.x;
        new_node->rightmost = new_node->location.x;
        new_node->upmost = new_node->location.y;
        new_node->downmost = new_node->location.y;

        new_node->left_idx = left_idx;
        new_node->right_idx = left_idx;
        
        return new_node;        
    }
    
    int axis = depth % 2;
    
    Node *new_node = (Node*)malloc(sizeof(Node));
    new_node->left_idx = left_idx;
    new_node->right_idx = right_idx;
    
    if (axis == 0)
    {
        qsort(pointlist, right + 1, sizeof(point), comparex);
        new_node->leftmost = pointlist[0].x;
        new_node->rightmost = pointlist[right].x;
    }
    else if (axis == 1)
    {
        qsort(pointlist, right + 1, sizeof(point), comparey);
        new_node->downmost = pointlist[0].y;
        new_node->upmost = pointlist[right].y;
    }
    
    int median = right / 2;

    unsigned int median_idx = (left_idx + right_idx + 1) / 2;
    
    new_node->location = pointlist[median];
    new_node->subtree_size = right + 1;    
    new_node->rightChild = kdtree(pointlist + median + 1, right - (median + 1), depth + 1, median_idx + 1, right_idx) ;
    new_node->leftChild = kdtree(pointlist, median, depth + 1, left_idx, median_idx - 1);
    
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
            new_node->downmost =  new_node->location.y;
        }        
    }
    else
    {        
        if (new_node->rightChild != NULL && new_node->leftChild != NULL)
        {
            new_node->rightmost = max(new_node->rightChild->rightmost,new_node->leftChild->rightmost);
            new_node->leftmost = min(new_node->rightChild->leftmost,new_node->leftChild->leftmost);
            
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
            new_node->leftmost =  new_node->location.x;
        }        
    }
    
    return new_node;    
}

// check fully covered
int is_contained(Node *region, unsigned int sx, unsigned int tx, unsigned int sy, unsigned int ty)
{
    unsigned int leftmost = region->leftmost;
    unsigned int rightmost = region->rightmost;
    unsigned int upmost = region->upmost;
    unsigned int downmost = region->downmost;    
    
    if (leftmost < sx || rightmost > tx || upmost > ty || downmost < sy) return 0;    
    return 1;
}

void ReportSubtree(Node *region, std::vector<std::pair<Node*, bool>> &result)
{   
    if (region->leftChild == NULL && (region->rightChild == NULL))
    {
        result.push_back({region, 0});
        return;
    }
    else if (region->subtree_size <= capacity)
    {
        result.push_back({region, 1});
        return;
    }
    else
    {
        result.push_back({region, 0});
    }
    
    if (region->leftChild != NULL) ReportSubtree(region->leftChild, result);
    if (region->rightChild != NULL) ReportSubtree(region->rightChild, result);
    
    return;
}

// orthogonal range search
void SearchKDtree(Node *v, std::vector<std::pair<Node*, bool>> &result, const unsigned int sx, const unsigned int tx, const unsigned int sy, const unsigned int ty)
{
    // disjoint case    
    if (v->rightmost < sx || v->leftmost > tx || v->downmost > ty || v->upmost < sy) return;
    
    // leaf node case
    if (v->leftChild == NULL && v->rightChild == NULL)
    {
        result.push_back({v, 0});
        return;
    }
    
    result.push_back({v, 0});
    
    // intermediate node case
    if (v->leftChild != NULL)
    {        
        if (is_contained(v->leftChild, sx, tx, sy, ty))
        {
            // ReportSubtree(v->leftChild, result);
            result.push_back({v->leftChild, 1});
            // std::cout << " subtree size: " << v->leftChild->subtree_size << "\n";
        }
        else
        {
            SearchKDtree(v->leftChild, result, sx, tx, sy, ty);
        }
    }
    
    if (v->rightChild != NULL)
    {        
        if (is_contained(v->rightChild, sx, tx, sy, ty))
        {
            // ReportSubtree(v->rightChild, result);
            result.push_back({v->rightChild, 1});
            // std::cout << " subtree size: " << v->rightChild->subtree_size << "\n";
        }
        else
        {
            SearchKDtree(v->rightChild, result, sx, tx, sy, ty);
        }
    }
    return;
}

// range counting
void CountKDtree(Node *v, unsigned int &count, const unsigned int sx, const unsigned int tx, const unsigned int sy, const unsigned int ty)
{
    // disjoint case    
    if (v->rightmost < sx || v->leftmost > tx || v->downmost > ty || v->upmost < sy) return;
    
    // leaf node case
    if (v->leftChild == NULL && v->rightChild == NULL)
    {        
        if (sx <= v->location.x && sy <= v->location.y && tx >= v->location.x && ty >= v->location.y)
        {
            ++count;
        }
        return;
    }
    
    // intermediate node case
    if (v->leftChild != NULL)
    {        
        if (is_contained(v->leftChild, sx, tx, sy, ty))
        {
            count += v->leftChild->subtree_size;
        }
        else
        {
            CountKDtree(v->leftChild, count, sx, tx, sy, ty);
        }
    }
    
    if (v->rightChild != NULL)
    {        
        if (is_contained(v->rightChild, sx, tx, sy, ty))
        {
            count += v->rightChild->subtree_size;
        }
        else
        {
            CountKDtree(v->rightChild, count, sx, tx, sy, ty);
        }
    }
    return;
}
