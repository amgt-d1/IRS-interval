## Note
* This is forked from [here](https://github.com/pbour/hint)
    * We extended the original code so that HINT<sup>m</sup> maintains all intervals overlapping a given query interval.
    * We measured only candidate computation time, as the sampling operation is totally the same as that of interval tree.
* To obtain query intervals, activate `output_query()` in another algorithm directory (e.g., interval tree).
    * For Book and BTC, we prepared query intervals in queryset.
* The format of datasets are different from our algorithms, use `.dat` and left- and right-endpoints are separeted by a single space.

## Dependencies
- g++/gcc
- Boost Library 

## Compile
Compile using ```make all```

## Execution
- ##### Examples
    ```
    $ ./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/BTC_norm.dat ../queryset/id-1_extent-0.08.qry
    ```

