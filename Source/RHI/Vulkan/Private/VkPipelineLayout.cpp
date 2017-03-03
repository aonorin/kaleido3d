#include "VkCommon.h"
#include "VkRHI.h"
#include "Private/VkUtils.h"

#include <map>

using namespace rhi::shc;
using namespace std;

K3D_VK_BEGIN

PipelineLayout::PipelineLayout(Device::Ptr pDevice, rhi::PipelineLayoutDesc const & desc)
	: DeviceChild(pDevice)
	, m_PipelineLayout(VK_NULL_HANDLE)
	, m_DescSetLayout(nullptr)
	, m_DescSet()
{
	InitWithDesc(desc);
}

PipelineLayout::~PipelineLayout()
{
	Destroy();
}

void PipelineLayout::Destroy()
{
	if (m_PipelineLayout == VK_NULL_HANDLE || !GetRawDevice())
		return;
	vkDestroyPipelineLayout(GetRawDevice(), m_PipelineLayout, nullptr);
	VKLOG(Info, "PipelineLayout Destroyed . -- %0x.", m_PipelineLayout);
	m_PipelineLayout = VK_NULL_HANDLE;
}

uint64 BindingHash(Binding const& binding)
{
	return (uint64)(1 << (3 + binding.VarNumber)) | binding.VarStage;
}

bool operator<(Binding const &lhs, Binding const &rhs)
{
	return rhs.VarStage < lhs.VarStage && rhs.VarNumber < lhs.VarNumber;
}

BindingArray ExtractBindingsFromTable(::k3d::DynArray<Binding> const& bindings)
{
//	merge image sampler
	std::map<uint64, Binding> bindingMap;
	for (auto const & binding : bindings)
	{
		uint64 hash = BindingHash(binding);
		if (bindingMap.find(hash) == bindingMap.end())
		{
			bindingMap.insert({ hash, binding });
		}
		else // binding slot override
		{
			auto & overrideBinding = bindingMap[hash];
			if (EBindType((uint32)overrideBinding.VarType | (uint32)binding.VarType) 
				== EBindType::ESamplerImageCombine)
			{
				overrideBinding.VarType = EBindType::ESamplerImageCombine;
			}
		}
	}

	BindingArray array;
	for (auto & p : bindingMap)
	{
		array.Append(RHIBinding2VkBinding(p.second));
	}
	return array;
}

void PipelineLayout::InitWithDesc(rhi::PipelineLayoutDesc const & desc)
{
	DescriptorAllocator::Options options;
	BindingArray array = ExtractBindingsFromTable(desc.Bindings);
	auto alloc = GetDevice()->NewDescriptorAllocator(16, array);
	m_DescSetLayout = GetDevice()->NewDescriptorSetLayout(array);
	m_DescSet = rhi::DescriptorRef( DescriptorSet::CreateDescSet(alloc, m_DescSetLayout->GetNativeHandle(), array, GetDevice()) );

	VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
	pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pPipelineLayoutCreateInfo.pNext = NULL;
	pPipelineLayoutCreateInfo.setLayoutCount = 1;
	pPipelineLayoutCreateInfo.pSetLayouts = &m_DescSetLayout->m_DescriptorSetLayout;
	K3D_VK_VERIFY(vkCreatePipelineLayout(GetRawDevice(), &pPipelineLayoutCreateInfo, nullptr, &m_PipelineLayout));
}

K3D_VK_END