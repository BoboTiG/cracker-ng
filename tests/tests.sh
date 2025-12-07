#!/bin/sh

echo " ZIP"
for file in files/zip/*.test; do
    echo -n " - ${file} ... "
    ./zipcracker-ng -f "${file}" -w list/10k-most-common.txt >/dev/null 2>&1 && echo -n "\033[0;32mOK\033[0m " || echo -n "\033[0;31mERROR\033[0m "
    ./zipcracker-ng -f "${file}" -w list/10k-most-common-crlf.txt >/dev/null 2>&1 && echo "\033[0;32mOK\033[0m" || echo "\033[0;31mERROR\033[0m"
done

echo -n " - files/zip/protected.zip ... "
echo "sample-files" | ./zipcracker-ng -f "files/zip/protected.zip" >/dev/null && echo "\033[0;32mOK\033[0m" || echo "\033[0;31mERROR\033[0m"

echo
echo "CPT"
for file in files/cpt/*.test; do
    echo -n " - ${file} ... "
    ./cptcracker-ng -f "${file}" -w list/10k-most-common.txt >/dev/null && echo -n "\033[0;32mOK\033[0m " || echo -n "\033[0;31mERROR\033[0m " 
    ./cptcracker-ng -f "${file}" -w list/10k-most-common-crlf.txt >/dev/null && echo "\033[0;32mOK\033[0m" || echo "\033[0;31mERROR\033[0m"
done
