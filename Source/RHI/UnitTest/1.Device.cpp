#include <Kaleido3D.h>
#include <Core/App.h>
#include <Core/Message.h>
#include <Interface/IRHI.h>
#include <Core/Module.h>
#include <RHI/Vulkan/Public/IVkRHI.h>
#include <RHI/Metal/Public/IMetalRHI.h>

using namespace k3d;

class UnitTestRHIDevice : public App
{
public:
	explicit UnitTestRHIDevice(kString const & appName)
		: App(appName, 1920, 1080)
	{}

	UnitTestRHIDevice(kString const & appName, uint32 width, uint32 height)
		: App(appName, width, height)
	{}

	bool OnInit() override;
	void OnDestroy() override;
	void OnProcess(Message & msg) override;

private:

	rhi::DeviceRef m_TestDevice;
};

K3D_APP_MAIN(UnitTestRHIDevice);

using namespace rhi;
using namespace k3d;

bool UnitTestRHIDevice::OnInit()
{
	App::OnInit();
#if K3DPLATFORM_OS_WIN || K3DPLATFORM_OS_ANDROID
	IVkRHI* pVkRHI = (IVkRHI*)ACQUIRE_PLUGIN(RHI_Vulkan);
	if (pVkRHI)
	{
		pVkRHI->Initialize("UnitTestRHIDevice", false);
		pVkRHI->Start();
		m_TestDevice = pVkRHI->GetPrimaryDevice();
	}
#else
    auto pMtlRHI = ACQUIRE_PLUGIN(RHI_Metal);
    if(pMtlRHI)
    {
        pMtlRHI->Start();
        m_TestDevice = ((IMetalRHI*)pMtlRHI)->GetPrimaryDevice();
    }
#endif
	return true;
}

void UnitTestRHIDevice::OnDestroy()
{
	App::OnDestroy();
}

void UnitTestRHIDevice::OnProcess(Message & msg)
{
}