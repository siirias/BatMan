#pragma once

#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <string>

float bm_rnd();
std::string wchar2str(const wchar_t* input);
wchar_t* str2wchar(std::string input);