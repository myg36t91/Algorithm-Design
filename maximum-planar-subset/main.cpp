#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;

vector<vector<int> > m; // 儲存 MIS() 遞迴的結果
vector<vector<bool> > b; // 紀錄 b[i][j] 是否是個有資料的區域

// MIS() 只負責把結果存到 m[i][j] 這個 2D array
int MIS(vector<int>&R, int i, int j){ // R 代表 i 到 j 的範圍
	
    // illegal case
    if(i>=j){
        b[i][j] = false;
        return 0;
    }
	
    if(m[i][j] != 0){
        return m[i][j];
    }
    else{
        // case 3: 與 j 相連的點剛好是 i，MIS(i,j) = MIS(i+1,j-1) + 1
        if(R[j]==i){
            m[i][j] = MIS(R, i+1, j-1) + 1;
            b[i][j] = true;
            return m[i][j];
        }
        // case 2: 與 j 相連的 k 點在 i ~ j 的範圍中，MIS(i,j) = max{(MIS(i,k-1) + MIS(k+1,j-1) + 1),MIS(i,j-1)}
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
    // case 1: 與 j 相連的 k 點不在 i ~ j 的範圍中，MIS(i,j) = MIS(i,j-1)
        m[i][j] = MIS(R, i, j-1);
        b[i][j] = false;
        return m[i][j];
    }
}

// output() 負責將結果輸出 
void output(fstream &fout, vector<int> &R, int i, int j){
    if(j>i){
       if(b[i][j] == true){
           // case 3
           if(R[j] == i){
               fout << i << " " << j << endl;
               output(fout, R, i+1, j-1);
           }
           // case 2
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
    // 檢查格式
    if(argc != 3){
        cout << "Usage: ./[exe] [input file] [output file]" << endl;
        exit(1);
    }
    // 開啟 .in 檔並檢查
    fstream fin;
    fin.open(argv[1], fstream::in);
    if(!fin.is_open()){
        cout << "Error: the input file is not opened!!" << endl;
        exit(1);
    }

    // 開啟 .out 檔並檢查
    fstream fout;
    fout.open(argv[2],fstream::out);
    if(!fout.is_open()){
        cout << "Error: the output file is not opened!!" << endl;
	    exit(1);
    }

    // 對 .in 檔進行解析
    char buffer[100];
    fin >> buffer;
    int numChord = atoi(buffer);

    cout << "============================" << endl;
    cout << "   Parser # Chords: " << numChord << endl;
    cout << "============================" << endl;

    vector<int> row_m; // 建立 1D array
    vector<bool> row_b;
    row_m.assign(numChord, 0);  // array 的大小根據 numChord 決定
    row_b.assign(numChord , 0);
    m.assign(numChord, row_m); // 建立出 numChord x numChord 大小的 2D array，m[i][j] 跟 b[i][j] 表格被建立
    b.assign(numChord, row_b);
    
    vector<int> data(numChord);
    for(int i = 0; i < numChord/2; i++){
    	fin >> buffer;
    	int start = atoi(buffer);
    	fin >> buffer;
    	int end = atoi(buffer);
    	cout << "Chord(" << start << "," << end << ")" << endl;
        data[start] = end;
        data[end] = start;
    }
    
    fout << MIS(data,0,numChord-1) << endl;
    output(fout,data,0,numChord-1);
    
    return 0;
}
