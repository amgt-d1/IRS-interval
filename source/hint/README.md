## Note
* This is forked from [here](https://github.com/pbour/hint)
    * We extended the original code so that HINT<sup>m</sup> maintains all intervals overlapping a given query interval.
    * We measured only candidate computation time, as the sampling operation is totally the same as that of interval tree.
* To obtain query intervals, activate `output_query()` in another algorithm directory (e.g., interval tree).

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile using ```make all```

## HINT<sup>m</sup>

### Source code files
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

### Execution
- ##### Examples
    ```
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/BTC_norm.dat ../queryset/id-1_extent-0.08.qry
    ```
