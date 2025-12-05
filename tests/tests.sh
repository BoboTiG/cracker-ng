#!/bin/sh

echo " ZIP"
for file in files/zip/*.test; do
    echo -n " - ${file} ... "
    ./zipcracker-ng -f "${file}" -w list/10k-most-common.txt >/dev/null && echo "\033[0;32mOK\033[0m" || echo "\033[0;31mERROR\033[0m"
done

echo
echo "CPT"
for file in files/cpt/*.test; do
    echo -n " - ${file} ... "
    ./cptcracker-ng -f "${file}" -w list/10k-most-common.txt >/dev/null && echo "\033[0;32mOK\033[0m" || echo "\033[0;31mERROR\033[0m"
done
