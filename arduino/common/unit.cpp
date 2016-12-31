#ifndef ARDUINO

#include <iostream>
#include "SerialUtils.h"


using namespace std;

int main(int arc, char* argv[])
{
	byte buffer[32];

	Serializer   out(buffer);
	Deserializer in (buffer);

	out << int(87654321) << "hello" << char('Z') << float(123.456);

	int i; char c; float f; char s[32];
	in >> i >> s >> c >> f;
	
	cout << i << " " << s << " " << c << " " << f << endl;

	return 0;
}

#endif // ARDUINO

