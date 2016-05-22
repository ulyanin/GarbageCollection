#ifndef GARBAGECOLLECTION_SMART_CLASSES_WITH_DERIVING_H
#define GARBAGECOLLECTION_SMART_CLASSES_WITH_DERIVING_H

namespace test1
{
    class A;

    class B : virtual public ISmartObject
    {
    public:
        A *pointer;

        virtual std::vector<ISmartObject *> pointers() const
        {
            return {(ISmartObject *) pointer};
        }
    };

    class A : virtual public ISmartObject
    {
    public:
        B b;

        A()
        {
            b.pointer = this;
        }

        virtual std::vector<ISmartObject *> pointers() const
        {
            return {};
        }
    };
}

namespace test2
{
    class A : virtual public ISmartObject
    {
    public:
        int f_a;
        A(int a=0) : f_a(a) {}
        virtual std::vector<ISmartObject *> pointers() const
        {
            return {};
        }
    };

    class B : virtual public ISmartObject
    {
    public:
        int f_b;
        B(int b=0) : f_b(b) {}
        std::vector<ISmartObject *> servants;
        void add_servant(ISmartObject * s)
        {
            servants.push_back(s);
        }

        virtual std::vector<ISmartObject *> pointers() const
        {
            return servants;
        }
    };

    class C : virtual public ISmartObject
    {
    public:
        A a_obj;
        C(int a): a_obj(a) { }
        virtual std::vector<ISmartObject *> pointers() const
        {
            return {};
        }
    };
}

namespace test3
{
    class A : virtual public ISmartObject
    {
    public:
        int f_a;
        A(int a=0) : f_a(a) {}
        std::vector<ISmartObject *> servants;
        void add_servant(ISmartObject * s)
        {
            servants.push_back(s);
        }

        virtual std::vector<ISmartObject *> pointers() const
        {
            return servants;
        }
    };
    class B : virtual public ISmartObject
    {
    public:
        int f_b;
        B(int b=0) : f_b(b) {}
        std::vector<ISmartObject *> servants;
        void add_servant(ISmartObject * s)
        {
            servants.push_back(s);
        }

        virtual std::vector<ISmartObject *> pointers() const
        {
            return servants;
        }
    };

    class C : virtual public ISmartObject
    {
    public:
        int f_c;
        C(int c=0) : f_c(c) {}
        std::vector<ISmartObject *> servants;
        void add_servant(ISmartObject * s)
        {
            servants.push_back(s);
        }

        virtual std::vector<ISmartObject *> pointers() const
        {
            return servants;
        }
    };
}

void test_smart_classes_with_deriving_1()
{
    std::cout << "test1: create class pointer to class A" << std::endl;
    new test1::A();
    GarbageCollection::getInstance().collectGarbage();
}

void test_smart_classes_with_deriving_2()
{
    std::cout << "test2: c -- object of class C that keeps class A is allocated on heap" << std::endl;
    std::cout << "test2: and b -- object of class B references to object of class A in c; "
              << "b allocated on stack" << std::endl;
    test2::C * c = new test2::C(3);
    test2::B b;
    b.add_servant(dynamic_cast<ISmartObject *>(&c->a_obj));
    ISmartObject * c_ptr = dynamic_cast<ISmartObject *>(c);
    std::cout << "test2: is c_ptr stores in Garbage Collector before collecting garbage: "
              << GarbageCollection::getInstance().isPointerSteelAlive(c_ptr) << std::endl;
    GarbageCollection::getInstance().collectGarbage();
    // both 1 is success!!
    std::cout << "test2: is c_ptr stores in Garbage Collector after collecting garbage: "
              << GarbageCollection::getInstance().isPointerSteelAlive(c_ptr) << std::endl;
}

void test_smart_classes_with_deriving_3()
{
    std::cout << "test3: c -- object of class C is owner of a -- pointer to object of class A" << std::endl;
    std::cout << "test3: b -- pointer to object of class B; a and b has cyclic references to each other" << std::endl;
    test3::C c(0);
    test3::A * a = new test3::A(1);
    test3::B * b = new test3::B(2);
    a->add_servant(b);
    b->add_servant(a);
}

#endif //GARBAGECOLLECTION_SMART_CLASSES_WITH_DERIVING_H
