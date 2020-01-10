#pragma once
#include "helper_functions.h"
#include "constants.h"

float bm_rnd()
{
	return ((float)rand())/((float)RAND_MAX);
}

std::string wchar2str(const wchar_t* input)
{
	std::string result = "";
	int i = 0;
	while (true)
	{
		const wchar_t *ch = (input + i++);
		result.append((const char*)ch);
		char check = (char)*ch;
		if (check == '\0')
			break;
	}
	return result;
}

wchar_t* str2wchar(std::string input)
{
	return (wchar_t*)input.c_str();
}