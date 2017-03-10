#include "VkCommon.h"
#include "VkRHI.h"
#include "Public/IVkRHI.h"
#include "Private/VkEnums.h"
#include "Private/VkUtils.h"
#include "VkConfig.h"

void InitializeVulkanRHI(const char* appName, bool debug)
{
	k3d::vk::RHIRoot::Initialize(appName, debug);
}

void DestroyVulkanRHI()
{
	k3d::vk::RHIRoot::Destroy();
}

namespace k3d
{
namespace vk
{

InstanceRef			RHIRoot::s_InstanceRef;
DynArray<GpuRef>	RHIRoot::s_GpuRefs;
DynArray<rhi::DeviceRef> RHIRoot::s_DeviceRefs;
RenderViewportSp	RHIRoot::s_Vp;

void dFunc(void*)
{
	RHIRoot::Destroy();
}

void RHIRoot::Initialize(const char * appName, bool debug)
{
	// create instance
	s_InstanceRef = InstanceRef(new vk::Instance(appName, appName, debug));
	s_GpuRefs = s_InstanceRef->EnumGpus();
	for (auto & gpu : s_GpuRefs)
	{
		auto pDevice = new Device;
		pDevice->Create(gpu->SharedFromThis(), s_InstanceRef->WithValidation());
		s_DeviceRefs.Append(rhi::DeviceRef( pDevice ));
	}
}

void RHIRoot::Destroy()
{
	VKLOG(Info, "RHIRoot::Destroy");
}

void RHIRoot::SetupDebug(VkDebugReportFlagsEXT flags, PFN_vkDebugReportCallbackEXT callBack)
{
	if (s_InstanceRef)
	{
		s_InstanceRef->SetupDebugging(flags, callBack);
	}
}

uint32 RHIRoot::GetHostGpuCount()
{
	return s_GpuRefs.Count();
}

GpuRef RHIRoot::GetHostGpuById(uint32 id)
{
	return s_GpuRefs[id];
}

VkInstance RHIRoot::GetInstance()
{
	return s_InstanceRef ? s_InstanceRef->m_Instance : VK_NULL_HANDLE;
}

rhi::DeviceRef RHIRoot::GetDeviceById(uint32 id)
{
	return s_DeviceRefs[id];
}

void RHIRoot::AddViewport(RenderViewportSp vp)
{
	s_Vp = vp;
}

RenderViewportSp 
RHIRoot::GetViewport(int index)
{
	return s_Vp;
}

void RHIRoot::EnumLayersAndExts()
{
	// Enum Layers
	uint32 layerCount = 0;
	K3D_VK_VERIFY(vkEnumerateInstanceLayerProperties(&layerCount, nullptr));
	if(layerCount > 0)
	{
		gVkLayerProps.Resize(layerCount);
		K3D_VK_VERIFY(vkEnumerateInstanceLayerProperties(&layerCount, gVkLayerProps.Data()));
	}

	// Enum Extensions
	uint32 extCount = 0;
	K3D_VK_VERIFY(vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr));
	if (extCount > 0)
	{
		gVkExtProps.Resize(extCount);
		K3D_VK_VERIFY(vkEnumerateInstanceExtensionProperties(nullptr, &extCount, gVkExtProps.Data()));
	}
	VKLOG(Info, ">> RHIRoot::EnumLayersAndExts <<\n\n"
		"=================================>> layerCount = %d.\n"
		"=================================>> extensionCount = %d.\n", layerCount, extCount);
}

} // End NS -- vk


#ifdef K3DPLATFORM_OS_WIN
static const char* LIBVULKAN = "vulkan-1.dll";
#elif defined(K3DPLATFORM_OS_MAC)
static const char* LIBVULKAN = "libvulkan.dylib";
#else
static const char* LIBVULKAN = "libvulkan.so";
#endif

class VkRHI : public IVkRHI
{
public:
	VkRHI()/* : m_VkLib(LIBVULKAN) */{}
	~VkRHI() override 
	{
		VKLOG(Info, "Destroying..");
		Shutdown();
	}

	void Initialize(const char* appName, bool debug) override
	{
		if (!m_IsInitialized)
		{
			k3d::vk::RHIRoot::Initialize(appName, debug);
			m_IsInitialized = true;
		}
		m_ConfigDebug = debug;
	}

	rhi::DeviceRef	GetPrimaryDevice() override 
	{
		return GetDeviceById(0);
	}
	
	uint32 GetDeviceCount() override { return m_DeviceCount; }
	
	rhi::DeviceRef	GetDeviceById(uint32 id) override
	{
		return vk::RHIRoot::GetDeviceById(id);
	}
	
	void Destroy() override
	{
		k3d::vk::RHIRoot::Destroy();
	}

	void Start() override
	{
		if (!m_IsInitialized)
		{
			VKLOG(Fatal, "RHI Module uninitialized!! You should call Initialize first!");
			return;
		}
	}

	void Shutdown() override
	{
		KLOG(Info, VKRHI, "Shuting down...");
		Destroy();
	}
	
	const char * Name() { return "RHI_Vulkan"; }

private:
	//dynlib::Lib				m_VkLib;

	bool					m_IsInitialized = false;
	uint32					m_DeviceCount = 0;
	bool					m_ConfigDebug = false;
};

}

MODULE_IMPLEMENT(RHI_Vulkan, k3d::VkRHI)
