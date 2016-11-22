#include <iostream>
#include "SerialUtils.h"


using namespace std;

int main(int arc, char* argv[])
{
	byte buffer[32];

	Serializer   out(buffer);
	Deserializer in (buffer);

	short rtab[] = {123, 456, 789};
	out << int(87654321) << String("hello") << char('Z') << float(123.456) << rtab;

	int i; char c; float f; String s; short ltab[3];
	in >> i >> s >> c >> f >> ltab;
	
	cout << i << " " << c << " " << f << " " << s << " {" << ltab[0] << ", " << ltab[1] << ", " << ltab[2] << "} " << endl;

	return 0;
}
