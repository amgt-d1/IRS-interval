#!/bin/bash

cd ../
cd parameter

echo "" > dataset_id.txt
echo 0 > dataset_id.txt
echo "" > scalability.txt
echo 1 > scalability.txt

echo "" > domain_extent.txt
echo 0.02 > domain_extent.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS samples/AARHUS-BOOKS_2013.dat ../queryset/id-0_extent-0.02.qry

cd parameter
echo "" > domain_extent.txt
echo 0.04 > domain_extent.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS samples/AARHUS-BOOKS_2013.dat ../queryset/id-0_extent-0.02.qry

cd parameter
echo "" > domain_extent.txt
echo 0.08 > domain_extent.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS samples/AARHUS-BOOKS_2013.dat ../queryset/id-0_extent-0.08.qry

cd parameter
echo "" > domain_extent.txt
echo 0.16 > domain_extent.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS samples/AARHUS-BOOKS_2013.dat ../queryset/id-0_extent-0.16.qry

cd parameter
echo "" > domain_extent.txt
echo 0.32 > domain_extent.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS samples/AARHUS-BOOKS_2013.dat ../queryset/id-0_extent-0.32.qry

cd parameter
echo "" > domain_extent.txt
echo 0.08 > domain_extent.txt

echo "" > scalability.txt
echo 0.2 > scalability.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/books_norm-0.2.dat ../queryset/id-0_extent-0.08_scalability-0.2.qry

cd parameter
echo "" > scalability.txt
echo 0.4 > scalability.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/books_norm-0.4.dat ../queryset/id-0_extent-0.08_scalability-0.4.qry

cd parameter
echo "" > scalability.txt
echo 0.6 > scalability.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/books_norm-0.6.dat ../queryset/id-0_extent-0.08_scalability-0.6.qry

cd parameter
echo "" > scalability.txt
echo 0.8 > scalability.txt
cd ../
./query_hint_m.exec -m 10 -o all -q gOVERLAPS ../dataset/books_norm-0.8.dat ../queryset/id-0_extent-0.08_scalability-0.8.qry