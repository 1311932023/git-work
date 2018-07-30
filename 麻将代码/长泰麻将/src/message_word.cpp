#include "message_word.h"

const char* msg_word(int index)
{
	const char* word[] =
	{
		"错误,未定义",
		"当前未过庄，不能胡",
		"当前未过庄，不能操作",
		"明杠变公杠不算分",
		"平胡不炮胡",
		"弯杠过手",
		"剩余4张牌"

	};
	if (index < (int)(sizeof(word) / sizeof(word[0])))
	{
		return word[index];
	}
	return word[0];
}