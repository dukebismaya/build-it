#include <iostream>
#include <fstream>
using namespace std;

int main() {
    char sentence[100];

    ofstream outFile;
    outFile.open("program.txt");

    cout << "Enter a sentence: ";
    cin.getline(sentence, 100);

    outFile << sentence;
    outFile.close();

    cout << "Sentence has been written to the file." << endl;

    return 0;
}
