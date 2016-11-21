#include <iostream>

#include "Buffer.h"
#include "RawInputBuffer.h"

#define TEST_BUFFER          0
#define TEST_RAWINPUTBUFFER  1
#define TEST_RAWOUTPUTBUFFER 0

using namespace std;

int main(int arc, char* argv[])
{

#if TEST_BUFFER
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

#if TEST_RAWINPUTBUFFER
	RawInputBuffer<6> buffer;
	
	int in_i0 = 123456789;
	byte in_b0 = 123, in_b1 = 45;

	buffer.append((byte*)(&in_i0), sizeof(in_i0));
	buffer.append(in_b0);
	buffer.append(in_b1);
	cout << buffer.getSize() << endl;

	int out_i0;
	byte out_b0, out_b1;

	buffer >> out_i0 >> out_b0 >> out_b1;
	cout << out_i0 << " " << int(out_b0) << " " << int(out_b1) << endl;

#endif // TEST_RAWINPUTBUFFER

	return 0;
}
