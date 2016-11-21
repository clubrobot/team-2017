#include <iostream>

#include "Buffer.h"
#include "RawInputBuffer.h"
#include "RawOutputBuffer.h"

#define TEST_BUFFER      0
#define TEST_RAWBUFFERS  1

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

#if TEST_RAWBUFFERS
	RawOutputBuffer<64> outputBuffer;

	outputBuffer << int(87654321) << String("hello") << char('Z') << float(123.456);

	RawInputBuffer<32> inputBuffer(outputBuffer);

	int i; char c; float f; String s;
	inputBuffer >> i >> s >> c >> f;
	
	cout << i << " " << c << " " << f << " " << s <<endl;
#endif // TEST_RAWBUFFERS

	return 0;
}
