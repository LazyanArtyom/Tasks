g++ -std=c++17 -Wall -Wextra server.cpp include/processmanager.cpp include/process.cpp -o server.out
g++ -std=c++17 -Wall -Wextra client.cpp include/process.cpp include/processmanager.cpp -o client.out