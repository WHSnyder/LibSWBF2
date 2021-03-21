#pragma once
#include "Chunks/GenericChunk.h"
#include "Types/List.h"
#include "Chunks/LVL/common/PROP.h"
#include "wrld.INFO.h"

namespace LibSWBF2::Chunks::LVL::wrld
{
	using LibSWBF2::Chunks::LVL::wrld::INFO;
	using common::PROP;

	struct LIBSWBF2_API inst : public GenericChunk<"inst"_m>
	{
	public:
		INFO* p_Info;
		List<PROP*> m_OverrideProperties;

	public:
		void RefreshSize() override;
		void WriteToStream(FileWriter& stream) override;
		void ReadFromStream(FileReader& stream) override;

		String ToString() const override;
	};
}