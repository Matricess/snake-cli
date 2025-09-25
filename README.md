# Snake game

## Compile and run
```bash
g++ -o snake main.cpp -pthread;
./snake
```



## Run Tests
```bash
g++ -o my_tests snake_test.cpp -lgtest -lgtest_main -pthread;
./my_tests
```

**For MacOS**
clang++ -std=c++17 snake_test.cpp \
    -I/opt/homebrew/include \
    -L/opt/homebrew/lib \
    -lgtest -lgtest_main -pthread \
    -o my_test
