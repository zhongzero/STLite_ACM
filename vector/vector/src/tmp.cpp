#include<bits/stdc++.h>
using namespace std;
vector<int>G1,G2;
int main(){
	int *p=(int*)malloc(sizeof(int));
	new (p) int(2);
	new (p) int(3);
	cout<<*p<<endl;
	free(p);
	return 0;
}