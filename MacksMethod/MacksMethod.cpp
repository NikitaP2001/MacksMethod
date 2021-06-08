
#include <windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>
#include <iomanip> 
#include <fstream>
#include <cstdlib>
#include <iostream>
using namespace std;

void startup() {
    if (SetConsoleTitleW(_T("Macks method"))) {
    }
}

class Matrix {
public:
    Matrix(const char* srcfile) {
        init();
        step = 0;
        set(srcfile);
    }
    ~Matrix() {
        for (int i = 0; i < size; i++) {
            delete[] costs[i];
            delete[] signs[i];
        }
        delete[] costs;
        delete[] signs;
    }

    void out() {
        step++;
        cout << "Output: " << step << "\n"
            << std::setw(9) << "Costs table:\n";
        for (int i = 0; i < size; i++) {
            cout << std::setw(12);
            for (int j = 0; j < size; j++) {
                cout << costs[i][j] << "  ";

            }
            cout << "\n\n";
        }
        cout << setw(9) << "Signs:\n";
        for (int i = 0; i < size; i++) {
            cout << setw(12);
            for (int j = 0; j < size; j++) {
                if (signs[i][j] == 1) std::cout << "*  ";
                if (signs[i][j] == 2) std::cout << "^  ";
                if (signs[i][j] == 0) std::cout << "0  ";
            }
            cout << "\n\n";
        }
    }

    int calculate_res(int cost) {
        int res = 0;
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                if (signs[i][j] == 1) {
                    res = res + costscpy[i][j] * cost;
                }
            }
        }
        return res;
    }

protected:
    void init() {
        size = 5;
        costs = new int* [size];
        costscpy = new int* [size];
        signs = new int* [size];
        for (int i = 0; i < size; i++) {
            signs[i] = new int[size];
            costs[i] = new int[size];
            costscpy[i] = new int[size];
            memset(costs[i], 0, sizeof(costs[i]) * size);
            memset(signs[i], 0, sizeof(signs[i]) * size);
            memset(costscpy[i], 0, sizeof(costscpy[i]) * size);
        }
    }

    void clear_row(int row) {
        for (int i = 0; i < size; i++) {
            if (signs[row][i] == 2) {
                signs[row][i] = 0;
            }
        }
    }

    void set(const char* srcfile) {
        ifstream fin;
        fin.open(srcfile);
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                fin >> costs[i][j];
                costscpy[i][j] = costs[i][j];
            }
        }
        fin.close();
    }

    int size;
    int step;
    int** costs;
    int** costscpy;
    int** signs;
};

class MacksMatrix : public Matrix {
public:
    MacksMatrix(const char* srcfile) : Matrix(srcfile) {
        B = new int[size];
        A = new int[size];
        memset(A, 0, sizeof(A) * size);
        memset(B, 0, sizeof(B) * size);
        evaluate_and_output();
    }
private:
    void evaluate_and_output() {
        bool finish = false;
        cout << "Start matrix is:" << endl;
        out();
        sign_min_in_rows();
        out();
        while (!finish) {
            searching_second_min();
            out();
            if (step6_check_cols()) {
                cout << "Answer is: " << calculate_res(13) << endl;
                cout << "Final Matrix:" << endl;
                out();
                finish = true;
            }
            else {
                seperate_to_A_B();
                add_to_B_cols(searcing_min_sub());
                out();
                memset(A, 0, sizeof(A) * size);
                memset(B, 0, sizeof(B) * size);
            }
        }
    }

    void add_to_B_cols(int min) {
        cout << "Adding min sub to all B elements. Res is:" << endl;
        for (int i = 0; i < size; i++) {
            if (B[i])
                for (int j = 0; j < size; j++) {
                    costs[j][i] = costs[j][i] + min;
                }
        }
    }

    int searcing_min_sub() {
        int pos;
        int rowmin = INT_MAX;
        for (int rows = 0; rows < size; rows++) {
            pos = -1;
            for (int cols = 0; cols < size; cols++) {
                if (signs[rows][cols] == 1 && B[cols]) {
                    pos = cols;
                }
            }
            if (pos != -1) {
                int min = INT_MAX;
                for (int i = 0; i < size; i++) {
                    if (A[i] && signs[rows][i] != 2 && costs[rows][i] - costs[rows][pos] < min) {
                        min = costs[rows][i] - costs[rows][pos];
                    }
                }
                if (min < rowmin) rowmin = min;
            }
        }
        cout << "Min sub is " << rowmin << endl;
        return rowmin;
    }

    void seperate_to_A_B() {
        std::cout << "Seperating elements to A & B" << endl;
        int signed_;
        for (int i = 0; i < size; i++) {
            signed_ = 0;
            for (int j = 0; j < size; j++) {
                if (signs[j][i]) {
                    signed_++;
                }
            }
            if (signed_ > 1) {
                B[i] = 1;
            }
            else A[i] = 1;
        }
        cout << "B: " << endl;
        for (int i = 0; i < size; i++) {
            cout << B[i] << " ";
        }
        cout << endl << "A: " << endl;
        for (int i = 0; i < size; i++) {
            cout << A[i] << " ";
        }
        cout << endl;
    }

    bool searching_second_min() {
        int min;
        bool flag = false;
        for (int i = 0; i < size; i++) {
            clear_row(i);
            while ((min = find_second_min(i)) != -1) {
                flag = true;
                if (!checkcoll(min)) {
                    for (int j = 0; j < size; j++) {
                        signs[i][j] = 0;
                        signs[i][min] = 1;
                    }
                }
                else {
                    int temp = -1;
                    for (int j = 0; j < size; j++) {
                        if (signs[i][j] == 1) {
                            if (checkcoll(j) > 1) {
                                temp = 1;
                            }
                        }
                    }
                    if (temp == 1) {
                        signs[i][min] = 2;
                    }
                    else break;
                }
            }
        }
        return flag;
    }

    bool step6_check_cols() {
        bool temp = false;
        for (int i = 0; i < size; i++) {
            temp = false;
            for (int j = 0; j < size; j++) {
                if (signs[j][i] == 1) {
                    temp = true;
                }
            }
            if (!temp) return false;
        }
        return true;
    }


    int checkcoll(int col) {
        int count = 0;
        for (int i = 0; i < size; i++) {
            if (signs[i][col]) {
                count++;
            }
        }
        return count;
    }

    void sign_min_in_rows() {
        cout << "Signing by * min element in row" << endl;
        for (int i = 0; i < size; i++) {
            signs[i][find_min_in_row(i)] = 1;
        }
    }

    int find_second_min(int row) {
        int min = 0;
        for (int i = 0; i < size; i++) {
            if (signs[row][i] == 1) {
                min = i;
            }
        }
        for (int i = 0; i < size; i++) {
            if (!signs[row][i] && costs[row][i] <= costs[row][min]) {
                return i;
            }
        }
        return -1;
    }

    int find_min_in_row(int row) {
        int min = 0;
        for (int i = 0; i < size; i++) {
            if (costs[row][i] < costs[row][min] && !signs[row][i]) {
                min = i;
            }
        }
        return min;
    }

    int* B;
    int* A;

};



int main()
{
    startup();

    MacksMatrix _mmx("in.txt");

    system("pause");
}