#pragma once
#include "Vector3.h"

namespace LibSWBF2::Types
{
#pragma pack(push, 1)
	struct LIBSWBF2_EXP Vector4 : public Vector3
	{
		static const ChunkSize SIZE = sizeof(float_t) * 4;

		Vector4();
		Vector4(const float_t& x, const float_t& y, const float_t& z, const float_t& w);
		~Vector4();

		void WriteToStream(FileWriter& stream) override;
		void ReadFromStream(FileReader& stream) override;

		float_t m_W = 0;
	};
#pragma pack(pop)
}