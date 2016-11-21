#include <iostream>

#include "Buffer.h"
#include "RawInputBuffer.h"

#define TEST_BUFFER          0
#define TEST_RAWINPUTBUFFER  0
#define TEST_RAWOUTPUTBUFFER 0

using namespace std;

int main(int arc, char* argv[])
{

#ifdef TEST_BUFFER
	Buffer<4> buffer;
	buffer.append(32);
	buffer.append(12);
	buffer.append(buffer.asArray(), buffer.getSize());
	buffer[0] = 123;
	buffer.append(56);
	buffer[-1] = 46;
	for (int i = 0; i < buffer.getSize(); i++)
	{
		cout << int(buffer[i]) << " ";
	}
	cout << endl;
#endif // TEST_BUFFER

	return 0;
}
