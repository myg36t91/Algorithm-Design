#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

vector<vector<int> > m; // store MIS recursive result
vector<vector<bool> > b; // record if-else have chord

int MIS(vector<int>&R, int i, int j){ // vecotr R mean range in i(start) for j(end)

    // illegal case
    if(i>=j){
        b[i][j] = false;
        return 0;
    }

    if(m[i][j] != 0){
        return m[i][j];
    }
    else{
        // case 3: 與 j 相連的點剛好是 i
        // MIS(i,j) = MIS(i+1,j-1) + 1
        if(R[j]==i){
            m[i][j] = MIS(R, i+1, j-1) + 1;
            b[i][j] = true;
            return m[i][j];
        }
        // case 2: 與 j 相連的 k 點在 i ~ j 的範圍中
        // MIS(i,j) = max{(MIS(i,k-1) + MIS(k+1,j-1) + 1),MIS(i,j-1)}
        else if(R[j] > i && R[j] < j){
            int temp1 = MIS(R, i, R[j]-1) + MIS(R, R[j]+1, j-1) + 1;
            int temp2 = MIS(R, i, j-1);
            if(temp1 > temp2){
                m[i][j] = temp1;
                b[i][j] = true;
                b[R[j]][j] = true;
                return m[i][j];
            }
            else{
                m[i][j] = temp2;
                b[i][j] = false;
                return m[i][j];
            }
        }
    // case 1: 與 j 相連的 k 點不在 i ~ j 的範圍中
    // MIS(i,j) = MIS(i,j-1)
        m[i][j] = MIS(R, i, j-1);
        b[i][j] = false;
        return m[i][j];
    }
}

void output(fstream &fout, vector<int> &R, int i, int j){
    if(j>i){
       if(b[i][j] == true){
           // case 3
           if(R[j] == i){
               fout << i << " " << j << endl;
               output(fout, R, i+1, j-1);
           }
           // case 2 R[j] > i && R[j] < j
           else{
               output(fout, R, i, R[j]-1); // i ~ k-1
               fout << R[j] << " " << j << endl;
               output(fout, R, R[j]+1, j); // k+1 ~ j
           }
       }
       // case 1
        else{
            output(fout, R, i, j-1);
        }
    }
}

int main(int argc, char *argv[]){
    // check format
    if(argc != 3){
        cout << "Usage: ./[exe] [input file] [output file]" << endl;
        exit(1);
    }
    // open the input file
    fstream fin;
    fin.open(argv[1], fstream::in);
    if(!fin.is_open())
    {
        cout << "Error: the input file is not opened!!" << endl;
        exit(1);
    }

    fstream fout;
    fout.open(argv[2],fstream::out);
    if(!fout.is_open())
    {
        cout << "Error: the output file is not opened!!" << endl;
	    exit(1);
    }

    // parse the input file
    char buffer[100];
    fin >> buffer;
    int numChord = atoi(buffer);

    cout << "============================" << endl;
    cout << "   Parser # Chords: " << numChord << endl;
    cout << "============================" << endl;

    vector<int> row_m;
    vector<bool> row_b;
    row_m.assign(numChord, 0); 
    row_b.assign(numChord , 0);
    m.assign(numChord, row_m);
    b.assign(numChord, row_b);
    
    vector<int> data(numChord);
    for(int i = 0; i < numChord/2; i++){
    	fin >> buffer;
    	int point1 = atoi(buffer);
    	fin >> buffer;
    	int point2 = atoi(buffer);
    	cout << "Chord(" << point1 << "," << point2 << ")" << endl;
        data[point1] = point2;
        data[point2] = point1;
    }
    
    fout << MIS(data,0,numChord-1) << endl;
    output(fout,data,0,numChord-1);
    
    return 0;
}