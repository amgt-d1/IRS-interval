# This is folked from [here](https://github.com/pbour/hint)

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile using ```make all``` or ```make <option>``` where `<option>` can be one of the following:
   - lscan 
   - 1dgrid 
   - hint
   - hint_m 

## Shared parameters among all methods
| Parameter | Description | Comment |
| ------ | ------ | ------ |
| -? or -h | display help message | |
| -v | activate verbose mode; print the trace for every query; otherwise only the final report is displayed | |
| -q | set predicate type:<br>(1) basic relationships from Allen's algebra, "EQUALS", "STARTS", "STARTED", "FINISHES", "FINISHED", "MEETS", "MET", "OVERLAPS", "OVERLAPPED", "CONTAINS", "CONTAINED", "BEFORE" "AFTER"<br>(2) generalized overlaps, "gOVERLAPS", from ACM SIGMOD'22 publication  | basic predicates work only for the linear scan method, 1D-grid and for the most advanced HINT<sup>m</sup> variants, with SUBS+SORT+SS+CM or ALL optimizations; rest of the methods return 0 |
| -r | set the number of runs per query; by default 1 | in our experimental analysis set to 10 |


## HINT<sup>m</sup>: 

#### Source code files
- main_hint_m.cpp
- containers/relation.h
- containers/relation.cpp
- containers/offsets.h
- containers/offsets.cpp
- containers/offsets_templates.h
- containers/offsets_templates.cpp
- indices/hierarchicalindex.h
- indices/hierarchicalindex.cpp
- indices/hint_m.h
- indices/hint_m.cpp
- indices/hint_m_subs+sort.cpp
- indices/hint_m_subs+sopt.cpp
- indices/hint_m_subs+sort+sopt.cpp
- indices/hint_m_subs+sort+sopt+ss.cpp
- indices/hint_m_subs+sort+sopt+cm.cpp
- indices/hint_m_subs+sort+ss+cm.cpp
- indices/hint_m_all.cpp

#### Execution
| Extra parameter | Description | Comment |
| ------ | ------ | ------ |
| -m |  set the number of bits | 10 for BOOKS in the experiments |
| -o |  set optimizations to be used: "SUBS+SORT" or "SUBS+SOPT" or "SUBS+SORT+SOPT" or "SUBS+SORT+SOPT+SS" or "SUBS+SORT+SOPT+CM" or "SUBS+SORT+SS+CM" or "ALL"| omit parameter for base HINT<sup>m</sup>; "CM" for cache misses optimization |
| -t |  evaluate query traversing the hierarchy in a top-down fashion; by default the bottom-up strategy is used | currently supported only by base HINT<sup>m</sup> |

- ##### Examples

    ###### base with top-down
    ```sh
    $ ./query_hint_m.exec -m 10 -t -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### base with bottom-up
    ```sh
    $ ./query_hint_m.exec -m 10 -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### subs+sort (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o subs+sort -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### subs+sopt (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o subs+sopt -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### subs+sort+sopt (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o subs+sort+sopt -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### subs+sort+sopt+ss (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o subs+sort+sopt+ss -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### subs+sort+sopt+cm (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o subs+sort+sopt+cm -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### subs+sort+ss+cm optimizations  (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o subs+sort+sopt+cm -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    ```
    ###### all optimizations  (only bottom-up)
    ```sh
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS -r 10 samples/AARHUS-BOOKS_2013.dat samples/AARHUS-BOOKS_2013_20k.qry
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS samples/AARHUS-BOOKS_2013.dat ../queryset/id-0_extent-0.08.qry
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/BTC_norm.dat ../queryset/id-1_extent-0.08.qry
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/nyc_bike_unixtime_norm.dat ../queryset/id-2_extent-0.08.qry
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/taxi_ecml_norm.dat ../queryset/id-3_extent-0.08.qry

    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/books_norm-0.2.dat ../queryset/id-0_extent-0.08_scalability-0.2.qry
    ```

