#pragma once

namespace VS
{
	enum class VSMessageType
	{
		Invalid,

		Name,

		Execute,
		Upload,

		Answer,
		Question,

		Finished, // Tag to mark transaction complete

		PerformanceHardware
	};

	VSMessageType ConvertMessageType(unsigned char messageType);
	unsigned char  ConvertMessageType(VSMessageType messageType);
}