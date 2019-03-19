#include <iostream>

using namespace std;

int main(int argc, char *argv[]){
	int sum = 0;

	for (int i = 0; i < 100; i++){
		sum += 10;
	}

	cout << "sum is " << sum << endl;

	return 0;
}
