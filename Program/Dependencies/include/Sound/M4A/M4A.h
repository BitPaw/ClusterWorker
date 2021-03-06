#pragma once

#include "../ISoundFormat.hpp"

namespace BF
{
	// mpeg-4 audio file
	struct M4A : public ISoundFormat
	{
		public:
		FileActionResult Load(const wchar_t* filePath);
		FileActionResult Save(const wchar_t* filePath);
		FileActionResult ConvertTo(Sound& sound);
		FileActionResult ConvertFrom(Sound& sound);
	};
}