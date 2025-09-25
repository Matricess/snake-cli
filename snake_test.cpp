#include <gtest/gtest.h>
#include "snake.h"


TEST(SnakeBehaviour, NextHeadRight) {
    pair<int, int> current = make_pair(rand() % 10, rand() % 10);
    EXPECT_EQ(get_next_head(current, 'r'),make_pair(current.first,current.second+1));
    
}


TEST(SnakeBehaviour, NextHeadLeft) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'l'),make_pair(current.first,current.second-1));
  
}

TEST(SnakeBehaviour, NextHeadUp) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'u'),make_pair(current.first-1,current.second));
}

TEST(SnakeBehaviour, NextHeadDown) {
  pair<int, int> current = make_pair(rand() % 10, rand() % 10);
  EXPECT_EQ(get_next_head(current, 'd'),make_pair(current.first+1,current.second));
  
}

// Food generation tests
TEST(FoodGeneration, NeverOnSnake){
  deque<pair<int,int>> snake;
  // occupy some cells
  snake.push_back(make_pair(0,0));
  snake.push_back(make_pair(0,1));
  snake.push_back(make_pair(1,0));
  snake.push_back(make_pair(1,1));
  pair<int,int> food = generate_food(10, snake);
  EXPECT_TRUE(find(snake.begin(), snake.end(), food) == snake.end());
}

// Score tests
TEST(Score, DefaultPoints){
  EXPECT_EQ(compute_score(-1), 0);
  EXPECT_EQ(compute_score(0), 0);
  EXPECT_EQ(compute_score(1), 10);
  EXPECT_EQ(compute_score(5), 50);
}

TEST(Score, CustomPointsPerFood){
  EXPECT_EQ(compute_score(3, 7), 21);
  EXPECT_EQ(compute_score(10, 1), 10);
}

TEST(FoodGeneration, HandlesNearlyFullBoard){
  int size = 3;
  deque<pair<int,int>> snake;
  // Fill all but one cell
  for(int i=0;i<size;i++){
    for(int j=0;j<size;j++){
      if (!(i==size-1 && j==size-1)){
        snake.push_back(make_pair(i,j));
      }
    }
  }
  pair<int,int> food = generate_food(size, snake);
  EXPECT_EQ(food, make_pair(size-1, size-1));
}

TEST(FoodGeneration, FullBoardReturnsSentinel){
  int size = 2;
  deque<pair<int,int>> snake;
  for(int i=0;i<size;i++){
    for(int j=0;j<size;j++){
      snake.push_back(make_pair(i,j));
    }
  }
  pair<int,int> food = generate_food(size, snake);
  EXPECT_EQ(food, make_pair(-1,-1));
}


// New tests for difficulty helpers
TEST(Difficulty, ComputeLevelThresholds){
  EXPECT_EQ(compute_level(-1), 0);
  EXPECT_EQ(compute_level(0), 0);
  EXPECT_EQ(compute_level(9), 0);
  EXPECT_EQ(compute_level(10), 1);
  EXPECT_EQ(compute_level(19), 1);
  EXPECT_EQ(compute_level(20), 2);
}

TEST(Difficulty, ComputeDelayReductionAndMinCap){
  // Defaults: base=500, reduce=100, min=100
  EXPECT_EQ(compute_delay_ms(0), 500);
  EXPECT_EQ(compute_delay_ms(1), 400);
  EXPECT_EQ(compute_delay_ms(2), 300);
  EXPECT_EQ(compute_delay_ms(3), 200);
  EXPECT_EQ(compute_delay_ms(4), 100); // hits min
  EXPECT_EQ(compute_delay_ms(5), 100); // stays at min

  // Custom parameters
  EXPECT_EQ(compute_delay_ms(3, 600, 50, 200), 450);
  EXPECT_EQ(compute_delay_ms(10, 600, 50, 200), 200); // min cap
}


/** 
 * g++ -o my_tests snake_test.cpp -lgtest -lgtest_main -pthread;
 * This command is a two-part shell command. Let's break it down.

  The first part is the compilation:
  g++ -o my_tests hello_gtest.cpp -lgtest -lgtest_main -pthread


   * g++: This invokes the GNU C++ compiler.
   * -o my_tests: This tells the compiler to create an executable file named
     my_tests.
   * hello_gtest.cpp: This is the C++ source file containing your tests.
   * -lgtest: This links the Google Test library, which provides the core testing
     framework.
   * -lgtest_main: This links a pre-compiled main function provided by Google
     Test, which saves you from writing your own main() to run the tests.
   * -pthread: This links the POSIX threads library, which is required by Google
     Test for its operation.
 * 
*/