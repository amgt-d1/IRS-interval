## Note
* This is folked from [here](https://github.com/pbour/hint)
* To obtain query intervals, activate `output_query()` in another algorithm directory (e.g., interval tree).

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile using ```make all```

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
    ###### all optimizations  (only bottom-up)
    ```
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/BTC_norm.dat ../queryset/id-1_extent-0.08.qry
    ```

