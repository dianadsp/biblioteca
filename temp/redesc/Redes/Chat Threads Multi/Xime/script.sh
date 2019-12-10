#! /bin/bash
if `g++ -o server2 server2.cpp -lpthread -std=c++11 -O3`
then
    echo fuser -n tcp 1300
    $(fuser -n tcp 1300)
	#./server2
else
echo "Error de compilacao";
fi

