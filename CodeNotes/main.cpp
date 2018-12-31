
#include <string>
#include <iostream>
#include "DynamicStorage.h"
#include "DynamicStorage.cpp"

using namespace std;

class StringThing {
public:
	string str;

	explicit StringThing(const string& string)
		: str(string) {}
};

int main(int argc, const char *argv[])
{
	{
		Dynamic::Storage<StringThing> thing;

		thing.append(StringThing("String1"));

		cout << thing.size() << endl;
		for (size_t i = 0; i < thing.size(); i++) cout << thing[i].str << ", ";
		cout << endl;

		thing.append(StringThing("String2"));

		cout << thing.size() << endl;
		for (size_t i = 0; i < thing.size(); i++) cout << thing[i].str << ", ";
		cout << endl;

		thing.append(StringThing("String3"));

		cout << thing.size() << endl;
		for (size_t i = 0; i < thing.size(); i++) cout << thing[i].str << ", ";
		cout << endl;

		//thing[17];

		Dynamic::Storage<StringThing> thing2 = thing;

		thing = thing2;

		cout << thing.size() << endl;
		for (size_t i = 0; i < thing.size(); i++) cout << thing[i].str << ", ";
		cout << endl;


		thing2.append(thing);

		cout << thing2.size() << endl;
		for (size_t i = 0; i < thing2.size(); i++) cout << thing2[i].str << ", ";
		cout << endl;

		cout << "Hi there!" << endl;
		cout << "argc = ," << argc << endl;
		for (int ii = 0; ii < argc; ++ii)
		{
			cout << "argv[" << ii << "] = " << argv[ii] << endl;
		}

		cout << "Press ENTER to continue...";
		cin.ignore();
	}
	return 0;
}