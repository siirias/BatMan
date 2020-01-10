#define _hypot hypot

#include <cmath>
#include <irrlicht.h>
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include "driverChoice.h"
#include<fstream>
#include<vector>
#include<list>
#include <sstream>
#include <iostream>   // std::cout
#include <string>     // std::string, std::stod
#if defined(_WIN32) || defined(_WIN64)
  #include<direct.h>
#endif
#include "BMApp.h"
#include "constants.h"
#include "Receiver.h"
#include "BathyData.h"
#include "BathyGraph.h"
#include "Graphics.h"
using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
struct SAppContext;

#ifdef _IRR_WINDOWS_
#pragma comment(lib, "Irrlicht.lib")
#endif


int main()
{
	BMApp BathyApp;
	BathyApp.init();
	while (BathyApp.GF.receiver!=NULL && BathyApp.GF.receiver->quitting==false)
	{
		BathyApp.refresh_app();
	}
	BathyApp.clear();
	return 0;
}


