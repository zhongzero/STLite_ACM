#include <iostream>
#include <vector>
 
using namespace std;
//第一种方式
template <class Iter>
struct iter_traits//简化版的iterator_traits，只定义了value_type；为了区别于STL中的iterator::traits，名字也改为iter_traits
{
    typedef typename Iter::value_type value_type;
};
iter_traits<A> B;
B.value_type

//第二种方式，偏特化
template <class Type>
struct iter_traits<Type *>
{
    typedef Type value_type2;
};
 
 
template <class Iter>
void iter_print(const Iter &a, const Iter &b)
{
    typedef typename iter_traits<Iter>::value_type2 T; //此处是关键，请思考一下：如果a，b是指针的话，除了用iter_traits的第二种方式，还有什么办法可以获得a或b所指向的数据的类型？
    T key;
    Iter p;
    for(p=a;p!=b;p++)
    {
        key = *p;
        cout << key << ",";
    }
    cout <<endl;
}
 
int main()
{
    int a[10] = {1,2,3,4,5,6,7,8,9,10};
    vector<string> b{"A","B","C","D","E","F","G"};
 
    iter_print<int *>(a,a+10);
    iter_print<vector<string>::iterator>(b.begin(), b.end());
    return 0;
}