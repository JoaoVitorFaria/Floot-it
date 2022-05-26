#include <bits/stdc++.h>
#include "termcolor.hpp"

using namespace std;
using namespace termcolor;

int n, colorCount = 0;
int counter;
int dx[4] = {0,-1,0,1};
int dy[4] = {1,0,-1,0};

// Check if a point is inside the puzzle boundary
bool valid(pair<int,int> p) {
    return p.first >= 0 && p.first < n && p.second >= 0 && p.second < n;
}

// Heuristic function: Number of regions in a puzzle, divided by number of colors.
double h(vector<vector<int>> a) {
    int region = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0 ; j < n; j++) {
            int color = a[i][j];
            if (color == 0) continue;   // counted
            region++;
            pair<int,int> p = {i,j};
            a[i][j] = 0;
            stack<pair<int,int>> s;
            s.push(p);
            while (!s.empty()) {
                pair<int,int> cur = s.top();
                s.pop();
                a[cur.first][cur.second] = 0;
                for (int i = 0; i < 4; i++) {
                    pair<int,int> next = {cur.first + dx[i], cur.second + dy[i]};
                    if (valid(next) && a[next.first][next.second] == color) {
                        s.push(next);
                    }
                }
            }
        }
    }
    return (double) region / (double) colorCount;
}

struct state {
    vector<vector<int>> puzzle;
    vector<int> moves;
    int heuristicValue;
    bool operator<(const state& b) const {
        return moves.size() + heuristicValue > b.moves.size() + b.heuristicValue;   // f(n) = g(n) + h(n)
    }
};

// Check if puzzle is solved
bool solved(state cur) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (cur.puzzle[i][j] != cur.puzzle[0][0]) return false;
        }
    }
    return true;
}

// Floodfill the top left region with a color
tuple<vector<vector<int>>, int> flood(vector<vector<int>> a, int after) {
    int before = a[0][0];
    int diff = 0;   // areas added after flooding
    pair<int,int> p = {0,0};
    stack<pair<int,int>> s;
    s.push(p);
    while (!s.empty()) {
        pair<int,int> cur = s.top();
        s.pop();
        a[cur.first][cur.second] = after;
        for (int i = 0; i < 4; i++) {
            pair<int,int> next = {cur.first + dx[i], cur.second + dy[i]};
            if (valid(next)) {
                if (a[next.first][next.second] == before) {
                    s.push(next);
                } else if (a[next.first][next.second] == after) {
                    diff++;
                }
            }
        }
    }
    return make_tuple(a, diff);
}

void printMoves(vector<int> moves) {
    for (int i = 0; i < moves.size(); i++) {
        if (moves[i] == 1) cout << red << moves[i] << reset << " ";
        else if (moves[i] == 2) cout << green << moves[i] << reset << " ";
        else if (moves[i] == 3) cout << yellow << moves[i] << reset << " ";
        else if (moves[i] == 4) cout << blue << moves[i] << reset << " ";
        else if (moves[i] == 5) cout << magenta << moves[i] << reset << " ";
        else if (moves[i] == 6) cout << cyan << moves[i] << reset << " ";
        else cout << white << moves[i] << reset << " ";
    }
    cout << endl;
}


vector<int> solveastar(vector<vector<int>> a) {
    priority_queue<state> q;
    counter = 0;
    state start;
    start.puzzle = a;
    start.moves = {};
    start.heuristicValue = h(a);

    counter++;
    q.push(start);
    
    while(!q.empty()) {
        state cur = q.top();
        q.pop();
        if (solved(cur)) {
            return cur.moves;
        }
        int currentColor = cur.puzzle[0][0];
        for (int i = 1; i <= colorCount; i++) {
            if (i == currentColor) continue;    // no use coloring with same color
            state next;
            int diff;
            next.moves = cur.moves;
            next.moves.push_back(i);
            tie(next.puzzle, diff) = flood(cur.puzzle, i);
            counter++;
            if (diff > 0) {     // no need to push "useless" moves (moves that do not increase area)
                next.heuristicValue = h(next.puzzle);
                q.push(next);
            }
        }
    }

    return {};
}


int main (int argc, char *argv[]) {
    string filename;
    if (argc == 1) {
        cout << "Enter filename: ";
        cin >> filename;
        cout << '\n';
    } else {
        filename = argv[1];
    }

    filename = "test/" + filename;

    ifstream fin(filename);
    if (!fin.good()) {
        cout << "File does not exist. Please check again." << endl;
        return 0;
    }

    vector<vector<int>> puzzle;
    fin >> n;
    for (int i = 0;i < n; i++) {
        vector<int> a;
        for (int j = 0; j < n; j++) {
            int x;
            fin >> x;
            colorCount = max(x, colorCount);
            a.push_back(x);
        }
        puzzle.push_back(a);
    }
    vector<int> astarMoves;
    clock_t start = clock();
    astarMoves = solveastar(puzzle);
    double astarTime = (clock()-start)*1./CLOCKS_PER_SEC;
    int astarStates = counter;

    cout << "A* Algorithm:" << endl;
    cout << "Time: " << fixed << setprecision(3) << astarTime << endl;
    cout << "Number of states: " << astarStates << endl;
    cout << "Number of moves: " << astarMoves.size() << endl;
    printMoves(astarMoves);
    cout << endl;
    return 0;
}