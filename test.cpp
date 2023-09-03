#include <iostream>
#include <typeinfo>

class MyClass {
public:
    // 类的成员和方法
};

int main() {
    MyClass obj;

    // 使用typeid操作符获取类的名称
    const std::type_info& typeInfo = typeid(obj);
    std::cout << "Class name: " << typeInfo.name() << std::endl;

    return 0;
}
