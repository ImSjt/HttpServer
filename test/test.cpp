#include <iostream>
#include <functional>
#include <memory>

using namespace std;

class Test
{
public:
    Test()
    {
        cout<<"construct"<<endl;
    }

    Test(const Test& t)
    {
        cout<<"copy construct"<<endl;
    }

    Test(const Test&& t)
    {
        cout<<"copy && construct"<<endl;
    }

    ~Test()
    {
        cout<<"destruct"<<endl;
    } 

    void print()
    {
        cout<<"test"<<endl;
    }
};

int main()
{
    std::function<void()> f;
    std::shared_ptr<Test> t(new Test);
    cout<<"use count:"<<t.use_count()<<endl;
    f = std::bind(&Test::print, t);
    cout<<"use count:"<<t.use_count()<<endl;
    f();

    return 0;
}

/*
construct
-----------------
copy construct
copy construct
copy construct
destruct
destruct
copy construct
destruct
destruct
destruct

construct
-----------------
copy construct
copy construct
copy construct
destruct
destruct
destruct
destruct
*/