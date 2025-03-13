//Please run this code with c++20 or later, else it will give error
//I am assuming that speed means how many units it can move in 1 move
//Also I am assuming it first moves to square, then changes speed, then direction
//and then moves accordingly
#include <bits/stdc++.h>
using namespace std;
//We are considering a combination of position, speed, direction and c_score as node
//The first int stores the number min no. of moves taken to reach this node
#define node tuple<int, pair<int, int>, int, int, int>
//lane_map stores the map, dp stores the no of 1's and 2's from origin till the point
vector<vector<int>> lane_map, dp;
map<tuple<pair<int, int>, int, int, int>, tuple<pair<int, int>, int, int, int>> visited;

void inputLaneMap() {
    int n, m;
    cout << "Enter number of rows and columns: ";
    cin >> n >> m;
    
    lane_map.assign(n, vector<int>(m)); // Resize lane_map to n x m

    cout << "Enter the grid values (0 for empty, 1 for obstacle, 2 for wall, 3 for start position):\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cin >> lane_map[i][j];
        }
    }
}

pair<int, int> getStartPos() {
    int n = lane_map.size();
    int m = lane_map[0].size();

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            if (lane_map[i][j] == 3) {
                return {i, j};
            }
        }
    }
    return make_pair(-1, -1);
}

//Checks if the move from old pos to new_pos is valid
bool validMove(pair<int, int> old_pos, pair<int, int> new_pos) {
    //check if the new_pos lies in the map
    if (new_pos.first < 0 || new_pos.second < 0)
        return false;
    if (new_pos.first >= lane_map.size() || new_pos.second >= lane_map[0].size())
        return false;

    //Used dynamic programming to see if the number of obstacles in the
    //rectangle formed by these diagonally opposite points is > 0 or not
    int x_max = max(old_pos.first, new_pos.first);
    int y_max = max(old_pos.second, new_pos.second);
    int x_min = min(old_pos.first, new_pos.first);
    int y_min = min(old_pos.second, new_pos.second);

    //if the number of obstacles in the rectangle formed by the 2 points is 0
    // return true else return false
    return (dp[x_max][y_max] + dp[x_min-1][y_min-1] - dp[x_max][y_min-1] - dp[x_min-1][y_max] == 0);
}

//Will be run initially to populate the dp
void populateDP() {
    int n = lane_map.size(), m = lane_map[0].size();
    dp.assign(n, vector<int>(m, 0));
    
    for (int i = 1; i < n; ++i) {
        for (int j = 1; j < m; ++j) {
            dp[i][j] = dp[i - 1][j] + dp[i][j - 1] - dp[i - 1][j - 1];
            if (lane_map[i][j] == 2 || lane_map[i][j] == 1) dp[i][j] += 1;
        }
    }
}

/* I have divided the map into 8 octants and assigned each of them
a number
North_west: 0
north: 1
north_east: 2
east: 3
south_east: 4
south: 5
south_west: 6
west: 7
*/
int getOctant(pair<int, int> pos){
	if(pos.first<3){
		if(pos.second<3)
			return 0;
		else if(pos.second>=3 && pos.second<lane_map[0].size()-3)
			return 7;
		else 
			return 6;
	}else if(pos.first>=3 && pos.first<lane_map.size()-3){
		if(pos.second>=lane_map[0].size()-3)
			return 5;
		else 
			return 1;
	}else{
		if(pos.second>=lane_map[0].size()-3)
			return 4;
		else if(pos.second<3)
			return 2;
		else
			return 3;
	}
}

//This function checks if resulting change of octant is in clockwise
//or anti-clockwise direction. If clockwise add 1 to c_score if anti
//add -1. if same octant than 0
int cScoreDiff(pair<int, int> old_pos, pair<int, int> new_pos) {
	int old_octant = getOctant(old_pos);
	int new_octant = getOctant(new_pos);
	if(new_octant-old_octant == 1 || new_octant-old_octant == -7) {
        return 1;
	}else if(new_octant == old_octant)
        return 0;
    else
        return -1;
}

//This is the main driving function
//It returns the minimum number of moves
tuple<pair<int,int>, int, int, int> generatePath() {
    //Node is defined on top
    queue<node> q;//We will the nodes in this in order of increasing moves from starting point
    //The cardinal directions in which the bot can move
    pair<int, int> dir_vecs[4] = {{0, 1}, {1, 0}, {0, -1}, {-1, 0}};
    //Get the starting position of the bot
    pair<int, int> start_pos = getStartPos();
    //No of moves to reach the initial mode is 0
    q.push(make_tuple(0, start_pos, 0, 0, 0));
    q.push(make_tuple(0, start_pos, 0, 1, 0));
    q.push(make_tuple(0, start_pos, 0, 2, 0));
    q.push(make_tuple(0, start_pos, 0, 3, 0));
    
    
    //The map stores whether we have added a node in queue or not
    //by storing the parent from which we have come on this node
    //This will also help us in back tracing later on
    auto init_parent = make_tuple(make_pair(-1,-1),0,0,0);
    visited[make_tuple(start_pos, 0, 0, 0)] = init_parent;
    visited[make_tuple(start_pos, 0, 1, 0)] = init_parent;
    visited[make_tuple(start_pos, 0, 2, 0)] = init_parent;
    visited[make_tuple(start_pos, 0, 3, 0)] = init_parent;

    while (!q.empty()) {
        //Get the top node in the queue and split it
        auto next_node = q.front(); q.pop();
        int moves = get<0>(next_node);
        pair<int, int> pos = get<1>(next_node);
        int speed = get<2>(next_node);
        int dir = get<3>(next_node);
        //c_score basically means how much of a clock_wise rotation we have completed
        //so basically the processing can only end when the score is 8
        //because we have divided the map into 8 quadrants and change in quadrant
        //clockwise results in +1. check cScoreDiff()
        int c_score = get<4>(next_node);

        auto parent = make_tuple(pos,speed,dir,c_score);

        //We can keep the speed constant or change it by one
        for (int s : {speed - 1, speed, speed + 1}) {
            if (s < 0 || s > sqrt(max((int)lane_map.size(), (int)lane_map[0].size())))
                continue;

            //-1, 0, 1 since we can stay in the same row or change it in both direction
            for (int i : {-1, 0, 1}) {
                //Add speed * direction vector to find new pos. Also considering row change using i
                //dir+1 because it is always perp to current direction
                int new_x = pos.first + speed * dir_vecs[dir].first + i * dir_vecs[(dir + 1) % 4].first;
                int new_y = pos.second + speed * dir_vecs[dir].second + i * dir_vecs[(dir + 1) % 4].second;
                pair<int, int> new_pos = {new_x, new_y};

                //check validMove defn above
                //basically checks if new_pos is a valid position
                //and there are no obstacles in between old and new position
                if (validMove(pos, new_pos)) {
                    int new_c_score = c_score + cScoreDiff(pos, new_pos);
                    if (!visited.contains(make_tuple(new_pos, s, dir, new_c_score))) {
                        //Check if we have reached the starting node after a complete round
                        if (s == 0 && new_pos == start_pos && new_c_score == 8) return parent;

                        //Push this new node in the queue if not visited
                        q.push({moves + 1, new_pos, s, dir, new_c_score});
                        //set visited to true
                        visited[make_tuple(new_pos, s, dir, new_c_score)] = parent;

                        //If speed is 0 or 1 we can also change direction
                        //Also take them into consderation
                        if (s == 0) {
                            for (int new_dir : {0, 1, 2, 3}) {
                                auto new_pos_dir = make_tuple(new_pos, s, new_dir, new_c_score);
                                if (!visited.contains(new_pos_dir)) {
                                    visited[new_pos_dir] = parent;
                                    q.push({moves + 1, new_pos, s, new_dir, new_c_score});
                                }
                            }
                        } else if (s == 1) {
                            for (int new_dir : {1, 3}) {
                                auto new_pos_dir = make_tuple(new_pos, s, (dir + new_dir) % 4, new_c_score);
                                if (!visited.contains(new_pos_dir)) {
                                    visited[new_pos_dir] = parent;
                                    q.push({moves + 1, new_pos, s, (dir + new_dir) % 4, new_c_score});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return make_tuple(make_pair(-1,-1),-1,-1,-1);
}

int main(){
    inputLaneMap();
    populateDP();
    auto start_pos = getStartPos();
    auto end = generatePath();
    //backtracing the path
    int count = 0;
    while(get<0>(end)!=start_pos){
        ++count;
        auto pos = get<0>(end);
        lane_map[pos.first][pos.second]=3;
        end = visited[end];
    }
    cout<<count<<'\n';
    for(int i=0; i<lane_map.size(); ++i){
        for(int j=0; j<lane_map[0].size();+ ++j){
            cout<<lane_map[i][j]<<' ';
        }
        cout<<'\n';
    }
    return 0;
}
