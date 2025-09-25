#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h> // for system clear
#include <map>
#include <deque>
#include <algorithm>
using namespace std;
using std::chrono::system_clock;
using namespace std::this_thread;
char direction='r';
bool paused=false;

// Pause helpers (purely for tests and input wiring)
inline void set_game_paused(bool value){ paused = value; }
inline void toggle_pause(){ paused = !paused; }
inline bool is_game_paused(){ return paused; }

// Difficulty helpers (pure functions for testing)
inline int compute_level(int food_eaten){
    if (food_eaten < 0) return 0;
    return food_eaten / 10;
}

inline int compute_delay_ms(int level, int base_delay_ms = 500, int per_level_reduction_ms = 100, int min_delay_ms = 100){
    if (level < 0) level = 0;
    int reduced = base_delay_ms - level * per_level_reduction_ms;
    return max(min_delay_ms, reduced);
}

// Score helper (pure)
inline int compute_score(int food_eaten, int points_per_food = 10){
    if (food_eaten <= 0) return 0;
    return food_eaten * points_per_food;
}


// Food generation helper: pick a free cell; returns (-1,-1) if none
inline pair<int,int> generate_food(int size, const deque<pair<int,int>> &snake){
    vector<pair<int,int>> free_cells;
    free_cells.reserve(size * size);
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            pair<int,int> cell = make_pair(i,j);
            if (find(snake.begin(), snake.end(), cell) == snake.end()){
                free_cells.push_back(cell);
            }
        }
    }
    if (free_cells.empty()){
        return make_pair(-1,-1);
    }
    int idx = rand() % free_cells.size();
    return free_cells[idx];
}


// Poison generation helper: pick a free cell not on snake and not on normal food; returns (-1,-1) if none
inline pair<int,int> generate_poison(int size, const deque<pair<int,int>> &snake, pair<int,int> food){
    vector<pair<int,int>> free_cells;
    free_cells.reserve(size * size);
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            pair<int,int> cell = make_pair(i,j);
            if (cell != food && find(snake.begin(), snake.end(), cell) == snake.end()){
                free_cells.push_back(cell);
            }
        }
    }
    if (free_cells.empty()){
        return make_pair(-1,-1);
    }
    int idx = rand() % free_cells.size();
    return free_cells[idx];
}

// Collision helper for tests
inline bool is_poison_collision(pair<int,int> head, pair<int,int> poison){
    return head == poison;
}


void input_handler(){
    // change terminal settings
    struct termios oldt, newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    // turn off canonical mode and echo
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    map<char, char> keymap = {{'d', 'r'}, {'a', 'l'}, {'w', 'u'}, {'s', 'd'}, {'q', 'q'}};
    while (true) {
        char input = getchar();
        if (keymap.find(input) != keymap.end()) {
            // This now correctly modifies the single, shared 'direction' variable
            direction = keymap[input];
        }else if (input == 'p' || input == 'P'){
            toggle_pause();
        }else if (input == 'q'){
            exit(0);
        }
        // You could add an exit condition here, e.g., if (input == 'q') break;
    }
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}


void render_game(int size, deque<pair<int, int>> &snake, pair<int, int> food, pair<int,int> poison){
    for(size_t i=0;i<size;i++){
        for(size_t j=0;j<size;j++){
            if (i == (size_t)food.first && j == (size_t)food.second){
                cout << "🍎";
            }else if (i == (size_t)poison.first && j == (size_t)poison.second){
                cout << "💀";
            }else if (find(snake.begin(), snake.end(), make_pair(int(i), int(j))) != snake.end()) {
                cout << "🐍";
            }else{
                cout << "⬜";
            }
    }
    cout << endl;
}
}

pair<int,int> get_next_head(pair<int,int> current, char direction){
    pair<int, int> next; 
    if(direction =='r'){
        next = make_pair(current.first,(current.second+1) % 10);
    }else if (direction=='l')
    {
        next = make_pair(current.first, current.second==0?9:current.second-1);
    }else if(direction =='d'){
            next = make_pair((current.first+1)%10,current.second);
        }else if (direction=='u'){
            next = make_pair(current.first==0?9:current.first-1, current.second);
        }
    return next;
    
}



void game_play(){
    system("clear");
    deque<pair<int, int>> snake;
    snake.push_back(make_pair(0,0));

    pair<int, int> food = generate_food(10, snake);
    pair<int, int> poison = generate_poison(10, snake, food);
    int food_eaten = 0;
    pair<int,int> head = make_pair(0,1);
    while(true){
        // send the cursor to the top
        cout << "\033[H";
        if (is_game_paused()){
            render_game(10, snake, food, poison);
            int level = compute_level(food_eaten);
            int score = compute_score(food_eaten, 10);
            cout << "length of snake: " << snake.size() << "  level: " << level << "  score: " << score << "  [PAUSED - press 'p' to resume]" << endl;
            sleep_for(chrono::milliseconds(100));
            continue;
        }
        pair<int,int> next_head = get_next_head(head, direction);
        // check self collision
        if (find(snake.begin(), snake.end(), next_head) != snake.end()) {
            system("clear");
            cout << "Game Over" << endl;
            exit(0);
        }else if (next_head.first == poison.first && next_head.second == poison.second){
            system("clear");
            cout << "Game Over (poison)" << endl;
            exit(0);
        }else if (next_head.first == food.first && next_head.second == food.second) {
            // grow snake
            snake.push_back(next_head);            
            food_eaten++;
            food = generate_food(10, snake);
            poison = generate_poison(10, snake, food);
        }else{
            // move snake
            snake.push_back(next_head);
            snake.pop_front();
        }
        render_game(10, snake, food, poison);
        int level = compute_level(food_eaten); // increase level every 10 apples
        int score = compute_score(food_eaten, 10);
        cout << "length of snake: " << snake.size() << "  level: " << level << "  score: " << score << endl;

        int current_delay_ms = compute_delay_ms(level, 500, 100, 100);
        sleep_for(chrono::milliseconds(current_delay_ms));
        head = next_head;
    }
}
