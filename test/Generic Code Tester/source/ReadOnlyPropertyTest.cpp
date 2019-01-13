#pragma once
#include <iostream>
#define PRINT(MSG) std::cout<<##MSG##<<std::endl;



template <typename T> class Property {
public:
	virtual ~Property() {}
	virtual T & operator = (const T & v) { return value = v; }
	virtual operator T const & () const { return value; }
protected:
	T value;
};





template <typename T> class ReadOnlyProperty{
public:
	virtual ~ReadOnlyProperty() {}
	virtual operator T const & () const { return value; }
protected:
	T value;
};



template <typename T> class InheritedReadOnlyProperty : public Property<T> {
public:
	virtual ~InheritedReadOnlyProperty() {}
	T & operator = (const T & v) override {};
};



template <typename T> class InheritedReadOnlyProperty : public Property<T> {
public:
	virtual ~InheritedReadOnlyProperty() {}
	//T & operator = (const T & v) override {};
};



//template <typename T> class PropertyReference {
//public:
//	virtual ~PropertyReference() {}
//	virtual operator Property<T> const & () const { return &value; }
//protected:
//	Property<T> * prop;
//};





class InheritedReadOnlyPropertyTest {
	class : public InheritedReadOnlyProperty<int> { friend class InheritedReadOnlyPropertyTest; } prop;
	InheritedReadOnlyProperty<int> prop1;

	InheritedReadOnlyPropertyTest(int i) {
		prop.value = 3;
		int j = prop;
		PRINT(i);
	}
};






inline void test(float i) {

}