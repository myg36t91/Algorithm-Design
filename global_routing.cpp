#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <stack>

using namespace std;

struct path{
	int distance;
	int path_row, path_col;
	int loop_num;
	int from_first, from_second;
};

struct mapping_pair{
	int x1, y1, x2, y2;
};

class global_routing{
	public:
    	void build_capacity(int);
		void build_net(int);
		void build_grid(int, int);
		void add_line(vector <int>);
		void set_out_file(const char*);
		void start();
        void relax(path*, path*, int);
        std::fstream fout;
        vector < vector<path> > grid;
		vector < vector<int> > lines;
		vector < vector<int> > edge_vertical;
		vector < vector<int> > edge_horiz;
		vector <path*> queue;
    private:
    	int capacity;
		int net_num, count;
		int row, col;
};

class sort_rule{
    public:
        static bool compare_distance(path*, path*);
};

#define BOUND 6666

// build capacity, net and grid
void global_routing :: build_capacity(int c){ capacity = c; }
void global_routing :: build_net(int n){ net_num = n; }
void global_routing :: build_grid(int grid_row, int grid_col){
	row = grid_row; col = grid_col;
	grid.resize(row); grid.resize(col);
	for(int i=0; i<row; i++){
		for(int j=0; j<col; j++){
			grid[i].push_back(path());
		}
	}
	for(int i=0; i<row; i++){
		for (int j=0; j<col; j++){
			grid[i][j].path_row = i;
			grid[i][j].path_col = j;
			grid[i][j].loop_num = 0;
		}
	}
	edge_vertical.resize(row-1);
	for(int i=0; i<row-1; i++){
		edge_vertical[i].resize(col);
	}
	edge_horiz.resize(row);
	for(int i=0; i<row; i++){
		edge_horiz[i].resize(col-1);
	}
}

void global_routing :: add_line(vector <int> temp){
    vector <int> t = temp;
	lines.push_back(t);
}

bool sort_rule :: compare_distance(path* element1, path* element2){
	return element1 -> distance < element2 -> distance;
}

void global_routing :: relax(path* u, path* v, int flg){
	int weight, mrow, mcol;
    if(flg==1){
        if(u -> path_row < v -> path_row){
            mrow = u -> path_row;
            mcol = u -> path_col;
        }
        else{
            mrow = v -> path_row;
            mcol = v -> path_col;           
        }
        weight = edge_vertical[mrow][mcol];
    }
    else{
        if(u -> path_col < v -> path_col){
            mrow = u -> path_row;
            mcol = u -> path_col;
        }
        else{
            mrow = v -> path_row;
            mcol = v -> path_col;
        }
        weight =  edge_horiz[mrow][mcol];
    }
	if(v -> loop_num != count){
		v -> distance = BOUND;
		v -> loop_num ++;
		queue.push_back(v);
	}
	if(v -> distance > u -> distance + weight){
		v -> distance = u -> distance + weight;
		v -> from_first = u -> path_row;
		v -> from_second = u -> path_col;
	}
}

void global_routing :: set_out_file(const char* filename){
	// open the output file
	fout.open(filename, std::fstream::out);
	if(!fout.is_open()){
	std::cout << "Error: the output file is not opened!!" << std::endl;
	    exit(1);
	}
}

void global_routing :: start(){
	count = 1;
	for(int i = 0; i < row-1; i++){
		for(int j = 0; j < col; j++){
			edge_vertical[i][j] = 0;
		}
	}
	for(int i = 0; i < row; i++){
		for(int j = 0; j < col-1; j++){
			edge_horiz[i][j] = 0;
		}
	}
	for(int i = 0; i < net_num; i++){
		if((i-1) >= 0){
            int tx = lines[(i-1)][3];
            int ty = lines[(i-1)][4];
            int curx, cury, curtmpx1, curtmpy1, curtmpx2, curtmpy2;
            curx = tx;
            cury = ty;

            while(true){
                curtmpx1 = grid[curx][cury].path_row;
                curtmpy1 = grid[curx][cury].path_col;
                curtmpx2 = grid[curx][cury].from_first;
                curtmpy2 = grid[curx][cury].from_second;
                if(curtmpx1 - curtmpx2 == 0){
                    if (curtmpy1 < curtmpy2){
                        edge_horiz[curtmpx1][curtmpy1]++;
                    }
                    else{
                        edge_horiz[curtmpx2][curtmpy2]++;
                    }
                }
                else{
                    if(curtmpx1 < curtmpx2){
                        edge_vertical[curtmpx1][curtmpy1]++;
                    }
                    else{
                        edge_vertical[curtmpx2][curtmpy2]++;
                    }
                }
                curx = curtmpx2;
                cury = curtmpy2;
                if(curx == lines[(i-1)][1] && cury == lines[(i-1)][2])
                    break;        
            }
        }

        int sx, sy;
        sx = lines[i][1];
        sy = lines[i][2];
        grid[sx][sy].distance = 0;
        grid[sx][sy].loop_num ++;
        queue.push_back(&grid[sx][sy]);

        while(!queue.empty()){
            path* u;
            sort(queue.begin(), queue.end(), sort_rule :: compare_distance);
            u = queue.front();
            queue.erase(queue.begin());
            
            //up
            if(u -> path_row-1 >= 0){
                relax(&grid[u -> path_row][u-> path_col], &grid[u -> path_row-1][u -> path_col], 1);
            }
            //down
            if(u -> path_row+1 < row){
                relax(&grid[u -> path_row][u -> path_col], &grid[u -> path_row+1][u -> path_col], 1);
            }
            //left
            if(u -> path_col-1 >= 0){
                relax(&grid[u -> path_row][u -> path_col], &grid[u -> path_row][u -> path_col-1], 0);
            }
            //right
            if(u -> path_col+1 < col){
                relax(&grid[u -> path_row][u -> path_col], &grid[u -> path_row][u -> path_col+1], 0);	
            }
        }

		mapping_pair mpr;
        int tx = lines[i][3];
        int ty = lines[i][4];
        int curx, cury, curtmpx, curtmpy;
        int mcount = 0;
        curx = tx;
        cury = ty;
        std::stack <mapping_pair> mapping_stack;

        while(true){
            mpr.x1 = grid[curx][cury].from_first;
            mpr.y1 = grid[curx][cury].from_second;
            mpr.x2 = grid[curx][cury].path_row;
            mpr.y2 = grid[curx][cury].path_col;
            mcount ++;
            mapping_stack.push(mpr);
            curtmpx = grid[curx][cury].from_first;
            curtmpy = grid[curx][cury].from_second;
            curx = curtmpx;
            cury = curtmpy;
            if (curx == lines[i][1] && cury == lines[i][2])
                break;
        }
        fout << i << " " << mcount << std::endl;
        while(!mapping_stack.empty()){
            fout << mapping_stack.top().x1 << " " << mapping_stack.top().y1 << " ";
            fout << mapping_stack.top().x2 << " " << mapping_stack.top().y2 << std::endl;
            mapping_stack.pop();
        }
		count++;
	}
}

int main(int argc, char** argv){
	global_routing gr;

    if(argc != 3){
        cout << "Usage: ./[exe] [input file] [output file]" << endl;
        exit(1);
    }

    fstream fin;
    fin.open(argv[1], fstream::in);
    if(!fin.is_open()){
        cout << "Error: the input file is not opened!!" << endl;
        exit(1);
    }

    char buffer[100];

    fin >> buffer;
    string str = buffer;
	if (str == "grid"){
		fin >> buffer; int row = atoi(buffer);
		fin >> buffer; int col = atoi(buffer);
		gr.build_grid(row, col);
    }

	fin >> buffer; str = buffer;
	if (str == "capacity"){
		fin >> buffer; int capacity = atoi(buffer);
		gr.build_capacity(capacity);
	}

	fin >> buffer; str = buffer;
	fin >> buffer; str = buffer;
	if (str == "net") {
		fin >> buffer; int nets = atoi(buffer);
		gr.build_net(nets);
	}

    vector <int> temp;
	for (int i = 0; i < nets; ++i) {
		fin >> buffer; int net_id = atoi(buffer);
		fin >> buffer; int start_x = atoi(buffer);
		fin >> buffer; int start_y = atoi(buffer);
		fin >> buffer; int to_x = atoi(buffer);
		fin >> buffer; int to_y = atoi(buffer);     	
        temp.push_back(net_id);
        temp.push_back(start_x); temp.push_back(start_y);
        temp.push_back(to_x); temp.push_back(to_y);
		gr.add_line(temp);
	}

	gr.set_out_file(argv[2]);
	gr.start();

	return 0;
}