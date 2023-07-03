#ifndef UTILS_H
#define UTILS_H

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <unistd.h>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_set>


// dataset info
unsigned int dataset_id = 0;
std::vector<std::pair<unsigned int, unsigned int>> intervals, intervals_ins;
std::vector<unsigned int> indices_del;
unsigned int domain_size = 0;
double domain_extent = 0;
long double domain_max = 0;
long double domain_min = 10000000000;
double scalability = 1.0;
unsigned int domain_exponent = 0;

// weight
std::vector<unsigned int> weights;

// sample size
unsigned int sample_size = 0;

// memory
double mem = 0;

// running time
double time_preprocess = 0;
double time_range_search = 0;
double time_range_sampling = 0;
double time_total_aggregate = 0;
double time_range_search_average = 0;
double time_range_sampling_average = 0;

// update stats.
double update_time = 0;
const unsigned int update_count = 5000;

// queries
const unsigned int query_size = 1000;
unsigned int result_size = 0;
double result_size_aggregate = 0;
double selectivity = 0;
std::vector<std::pair<unsigned int, unsigned int>> query_set;


// compute memory usage
double process_mem_usage()
{
    double resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
            >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    resident_set = rss * page_size_kb;

	return resident_set / 1000;
}

// parameter input
void input_parameter()
{
    std::ifstream ifs_dataset_id("parameter/dataset_id.txt");
    std::ifstream ifs_sample_size("parameter/sample_size.txt");
    std::ifstream ifs_extent("parameter/domain_extent.txt");
    std::ifstream ifs_scalability("parameter/scalability.txt");

    if (ifs_dataset_id.fail())
    {
        std::cout << " dataset_id.txt does not exist." << std::endl;
        std::exit(0);
    }
    if (ifs_sample_size.fail())
    {
        std::cout << " sample_size.txt does not exist." << std::endl;
        std::exit(0);
    }
    if (ifs_extent.fail())
    {
        std::cout << " domain_extent.txt does not exist." << std::endl;
        std::exit(0);
    }
    if (ifs_scalability.fail())
    {
        std::cout << " scalability.txt does not exist." << std::endl;
        std::exit(0);
    }

    while (!ifs_dataset_id.eof()) { ifs_dataset_id >> dataset_id; }
    while (!ifs_sample_size.eof()) { ifs_sample_size >> sample_size; }
    while (!ifs_extent.eof()) { ifs_extent >> domain_extent; }
    while (!ifs_scalability.eof()) { ifs_scalability >> scalability; }
}

// data input
void input_data()
{
    std::mt19937 mt;
    std::uniform_real_distribution<> rnd_prob(0, 1.0);

    // interval variable
    std::pair<unsigned int, unsigned int> interval;

    // dataset input
    std::string f_name = "../dataset/";
    if (dataset_id == 0) f_name += "books.csv";
    if (dataset_id == 1) f_name += "BTC.csv";
    if (dataset_id == 2) f_name += "nyc_bike_unixtime.csv";
    if (dataset_id == 3) f_name += "nyc_taxi.csv";

    // file input
    std::ifstream ifs_file(f_name);
    std::string full_data;

    // error check
    if (ifs_file.fail())
    {
        std::cout << " data file does not exist." << std::endl;
        std::exit(0);
    }

    // read data
    while (std::getline(ifs_file, full_data))
    {
        std::string meta_info;
        std::istringstream stream(full_data);
        std::string type = "";

        for (unsigned int i = 0; i < 2; ++i)
        {
            std::getline(stream, meta_info, ',');
            std::istringstream stream_(meta_info);
            long double val = std::stold(meta_info);
            if (i == 0) interval.first = (unsigned int)val;
            if (i == 1) interval.second = (unsigned int)val;
        }

        if (interval.first < interval.second)
        {
            if (domain_max < interval.second) domain_max = interval.second;
            if (domain_min > interval.first) domain_min = interval.first;
            if (rnd_prob(mt) <= scalability) intervals.push_back(interval);
        }
    }

    unsigned int d_max = 1;
    const unsigned int shift_dom = (unsigned int)(domain_max - domain_min);
    while (d_max < shift_dom)
    {
        d_max *= 2;
        ++domain_exponent;
    }

    /* show input parameters */
    std::cout << " -------------------------------\n";
    std::cout << " dataset ID: " << dataset_id << "\n";
    std::cout << " scalability [%]: " << scalability * 100 << "\n";
    std::cout << " cardinality: " << intervals.size() << "\n";
    std::cout << " domain (w/o shifting): [" << (unsigned int)domain_min << ", " << (unsigned int)domain_max << "]\n";
    std::cout << " domain (w/ shifting): [0, " << shift_dom << "] <= 2^" << domain_exponent << "\n";
    domain_size = (unsigned int)(domain_max - domain_min);
    std::cout << " domain size: " << domain_size << "\n";
    std::cout << " sample size: " << sample_size << "\n";
    std::cout << " extent: " << domain_extent << "\n";
    std::cout << " -------------------------------\n\n";

    // normalization
    const unsigned int size = intervals.size();
    for (unsigned int i = 0; i < size; ++i)
    {
        intervals[i].first -= domain_min;
        intervals[i].second -= domain_min;
    }
}

// input data (for insertion test)
void input_data_ins()
{
    // input data
    input_data();

    const unsigned int size = intervals.size();

    // use the last update_count intervals for insertions
    for (unsigned int i = 0; i < update_count; ++i) intervals_ins.push_back(intervals[size - i - 1]);

    // delete last update_count intervals from original
    for (unsigned int i = 0; i < update_count; ++i) intervals.pop_back();
}

// determine deletion data
void make_deletion()
{
    std::mt19937 mt;
    std::uniform_real_distribution<> rnd(0, intervals.size() -1);

    std::unordered_set<unsigned int> del;
    while (del.size() < update_count)
    {
        const unsigned int idx = rnd(mt);
        del.insert(idx);
    }

    auto it = del.begin();
    while (it != del.end())
    {
        indices_del.push_back(*it);
        ++it;
    }
}

// make weights
void make_weight()
{
    const unsigned int size = intervals.size();
    weights.resize(size);

    const unsigned int weight_max = 100;

    std::mt19937 mt;
    std::uniform_int_distribution<> rnd_weight(1, weight_max);
    for (unsigned int i = 0; i < size; ++i) weights[i] = rnd_weight(mt);
}

// query making function
void make_queries()
{
    std::mt19937 mt;
    std::uniform_int_distribution<> rnd_idx(0, intervals.size() - 1);

    for (unsigned int i = 0; i < query_size; ++i)
    {
        // get random index
        const unsigned int idx = rnd_idx(mt);

        // make query
        std::pair<unsigned int, unsigned int> q = {intervals[idx].first, intervals[idx].first + domain_size * domain_extent};
        query_set.push_back(q);
    }
}


#endif
