#include <iostream>
using namespace std;

void enterData(int firstMatrix[][10], int secondMatrix[][10], int r1, int c1, int r2, int c2);
void multiplyMatrices(int firstMatrix[][10], int secondMatrix[][10], int multResult[][10], int r1, int c1, int r2, int c2);
void display(int mult[][10], int r1, int c2);

int main() {
    int firstMatrix[10][10], secondMatrix[10][10], mult[10][10], r1, c1, r2, c2;

    cout << "Enter rows and columns for first matrix: ";
    cin >> r1 >> c1;
    cout << "Enter rows and columns for second matrix: ";
    cin >> r2 >> c2;

    while (c1 != r2) {
        cout << "Error! column of first matrix not equal to row of second." << endl;
        cout << "Enter rows and columns for first matrix: ";
        cin >> r1 >> c1;
        cout << "Enter rows and columns for second matrix: ";
        cin >> r2 >> c2;
    }

    enterData(firstMatrix, secondMatrix, r1, c1, r2, c2);
    multiplyMatrices(firstMatrix, secondMatrix, mult, r1, c1, r2, c2);
    display(mult, r1, c2);

    return 0;
}

void enterData(int firstMatrix[][10], int secondMatrix[][10], int r1, int c1, int r2, int c2) {
    int i, j;
    cout << endl << "Enter elements of matrix 1:" << endl;
    for(i = 0; i < r1; ++i) {
        for(j = 0; j < c1; ++j) {
            cout << "Enter element a" << i + 1 << j + 1 << " : ";
            cin >> firstMatrix[i][j];
        }
    }

    cout << endl << "Enter elements of matrix 2:" << endl;
    for(i = 0; i < r2; ++i) {
        for(j = 0; j < c2; ++j) {
            cout << "Enter element b" << i + 1 << j + 1 << " : ";
            cin >> secondMatrix[i][j];
        }
    }
}

void multiplyMatrices(int firstMatrix[][10], int secondMatrix[][10], int mult[][10], int r1, int c1, int r2, int c2) {
    int i, j, k;

    for(i = 0; i < r1; ++i) {
        for(j = 0; j < c2; ++j) {
            mult[i][j] = 0;
        }
    }

    for(i = 0; i < r1; ++i) {
        for(j = 0; j < c2; ++j) {
            for(k = 0; k < c1; ++k) {
                mult[i][j] += firstMatrix[i][k] * secondMatrix[k][j];
            }
        }
    }
}

void display(int mult[][10], int r1, int c2) {
    int i, j;
    cout << endl << "Output Matrix:" << endl;
    for(i = 0; i < r1; ++i) {
        for(j = 0; j < c2; ++j) {
            cout << " " << mult[i][j];
            if(j == c2 - 1)
                cout << endl;
        }
    }
}
