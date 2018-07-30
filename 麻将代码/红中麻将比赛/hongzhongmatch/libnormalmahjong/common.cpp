#include "common.h"

int TypeConvert(int iOpType)
{
	if (iOpType & (TYPE_DATOUZI))
	{
		switch (iOpType)
		{
		case TYPE_DATOUZI_ZHI: return TYPE_ZHIGANG;
		case TYPE_DATOUZI_AN: return TYPE_ANGANG;
		case TYPE_DATOUZI_WAN: return TYPE_WANGANG;
		}
	}
	return iOpType;
}
