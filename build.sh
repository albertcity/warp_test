set -e
set -x
g++ -std=c++11 -g -Wall -Werror -O2 warp_test.cc -o warp_test \
  -I . \
  `pkg-config --cflags --libs opencv` \
  -DCXX
