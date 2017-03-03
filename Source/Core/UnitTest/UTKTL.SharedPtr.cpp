#include "Common.h"
#include <Core/CameraData.h>
#include <KTL/String.hpp>
#include <iostream>

#if K3DPLATFORM_OS_WIN
#pragma comment(linker,"/subsystem:console")
#endif

using namespace std;
using namespace k3d;

class SharedTest
{
public:
	SharedTest()
	{

	}
	~SharedTest()
	{
		cout << "SharedTest Destroy. " << endl;
	}
};

void TestSharedPtr()
{
	SharedPtr<Os::File> spFile(new Os::File);
	cout << "file:" << spFile.UseCount() << endl;

	SharedPtr<CameraData> spCam(new CameraData);
	spCam.UseCount();
	spCam->SetName("TestSharedPtr");

	SharedPtr<SharedTest> spTest(new SharedTest);
	cout << "SharedTest:" << spTest.UseCount() << endl;

	SharedPtr<IIODevice> ioFile = spFile;
	K3D_ASSERT(ioFile.UseCount()==2);
		
	{
		WeakPtr<IIODevice> weakRef(ioFile);
		cout << "weakREf:" << ioFile.UseCount() << endl;
	}

	auto makeShared = MakeShared<Os::File>();
	makeShared->Open(KT("TestSharedPtr"), IOWrite);

	auto refMakeShared = makeShared;
	cout << "refMakeShared:" << refMakeShared.UseCount() << endl;

	refMakeShared->Close();
}

int main(int argc, char**argv)
{
	TestSharedPtr();
	return 0;
}