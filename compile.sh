#!/usr/bin/env sh

g++ -fsanitize=address,undefined -std=c++20 -O2 -Wall -Wextra driver.cpp -o analyze
./analyze samples/menu3.menu samples/menu3.chosen                                  
diff <(./analyze samples/menu3.menu samples/menu3.chosen) samples/expected_menu3.txt
