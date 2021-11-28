#pragma once

namespace VS
{
	enum class VSMessageType
	{
		Invalid,

		Name,

		Execute,
		Upload,

		Get,
		Put
	};

	VSMessageType ConvertMessageType(unsigned char messageType);
	unsigned char  ConvertMessageType(VSMessageType messageType);
}