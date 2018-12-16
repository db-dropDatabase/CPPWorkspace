#include <iostream>

using namespace std;


class StringThing {
  public:
    const string str;

    explicit StringThing(const string string)
    // member initialization list! It's so cool!
    : str(string) {}

    int getLength(const string str) const {
      return str.length();
    }

    int getLength() const {
      return getLength(this->str);
    }

    int getLength() {
      return getLength(this->str);
    }
};

int main(int argc, const char *argv[])
{
  StringThing stringthing("House");
  const StringThing str("Table");

  cout << stringthing.str << endl;
  cout << stringthing.getLength() << endl;
  cout << str.getLength() << endl;

  cout << "Hi there!" << endl;
  cout << "argc = ," << argc << endl;
  for (int ii = 0; ii < argc; ++ii)
  {
    cout << "argv[" << ii << "] = " << argv[ii] << endl;
  }

  cout << "Press ENTER to continue...";
  cin.ignore();
  return 0;
}
