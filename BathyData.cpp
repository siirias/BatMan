#include "BathyData.h"


BathyData::BathyData()
{
	x_in_m = 2000.0f;
	y_in_m = 2000.0f;
	depth_in_m = 1.0f;
	depth_negative = false;
	undo_steps.clear();
	current_undo_step = -1;
	reset_metadata();
}


BathyData::~BathyData()
{
}

bool BathyData::LoadFromFile(std::string FileName, irr::IrrlichtDevice *device)
{
	std::cout << "Reading file: " << FileName <<"\n";
	if (LoadFromXMLFile(FileName,device))
	{
		std::cout << "XML metadata read succesfully.\n";
		return true;
	}
	//If XML reading didn't succeed, then let's try to read as regular ascii file.
	reset_metadata();
	std::cout << "No recognizable xml data, trying as pure ascii\n";
	depths.clear();
	std::fstream fp;
	fp.open(FileName, std::ios::in);
	if (!fp.is_open())
		return false;
	cell_sizes.clear();
	unsigned int x = 0;
	unsigned int y = 0;
	if (fp.is_open())
	{
		float number;
		std::string tmp;
		while (std::getline(fp, tmp, '\n'))
		{
			y = 0;
			std::string tmp2;
			std::istringstream iss(tmp);
			while (std::getline(iss, tmp2, ' '))
			{
				number = (float)atof(tmp2.c_str());
				if (!depth_negative)
					number *= -1.0f;
				if (number > 0.0f)
					number = 0.0f;
				depths.push_back(number);
				y++;
				cell_sizes.setX(bm_max(cell_sizes.getX(), x));
				cell_sizes.setY(bm_max(cell_sizes.getY(), y));
			}
			x++;
		}
		if (x == 0 || y == 0)
		{
			std::cout << "Empty File Read.\n";
			return false;
		}
		std::cout << "Ascii data  read succesfully.\n";
		std::cout << "Got " << x << "x" << y << " grid.\n";
	}
	else
		return false;
	fp.close();
	return true;
}

bool BathyData::LoadFromXMLFile(std::string FileName, irr::IrrlichtDevice *device)
{
	if (device == NULL)
		return false;
	irr::io::IXMLReader *xml = device->getFileSystem()->createXMLReader(irr::core::stringw(FileName.c_str()).c_str());
	if (!xml)
	{
		return false;
	}
	bool got_data = false;
	irr::core::stringw current_block = L"";
	while (xml->read())
	{
		switch (xml->getNodeType())
		{
			case irr::io::EXN_ELEMENT:
			{
				if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("depth_negative"))
				{
					depth_negative = xml->getAttributeValueAsInt(0)!=0;
				}
				if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("coordinates"))
				{
					for (unsigned int i = 0; i < xml->getAttributeCount(); i++)
					{
						if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("lat_min"))
							lat_min = xml->getAttributeValueAsFloat(i);
						if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("lat_max"))
							lat_max = xml->getAttributeValueAsFloat(i);
						if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("lon_min"))
							lon_min = xml->getAttributeValueAsFloat(i);
						if (((irr::core::stringw)(xml->getAttributeName(i))).equals_ignore_case("lon_max"))
							lon_max = xml->getAttributeValueAsFloat(i);
					}
				}
				if (((irr::core::stringw) xml->getNodeName()).equals_ignore_case("data"))
				{
					current_block = ((irr::core::stringw) xml->getNodeName());
				}
			}
			break;
			case irr::io::EXN_TEXT:
			{
				if (current_block.equals_ignore_case("data"))
				{
					std::istringstream data(wchar2str(xml->getNodeData()));
					depths.clear();
					cell_sizes.clear();
					unsigned int x = 0;
					unsigned int y = 0;
					float number;
					std::string tmp;
					while (std::getline(data, tmp, '\n'))
					{
						if (strspn(tmp.c_str(), "-.0123456789") == 0)
							continue; //No numbers in this line so skip
						y = 0;
						x++;
						std::string tmp2;
						std::istringstream iss(tmp);
						while (std::getline(iss, tmp2, ' '))
						{
							if (strspn(tmp2.c_str(), "-.0123456789") != tmp2.size())
								continue; //This is no number
							number = (float)atof(tmp2.c_str());
							if (!depth_negative)
								number *= -1.0f;
							if (number > 0.0f)
								number = 0.0f;
							depths.push_back(number);
							y++;
							cell_sizes.setX(bm_max(cell_sizes.getX(), x));
							cell_sizes.setY(bm_max(cell_sizes.getY(), y));
						}
					}
				}
				got_data = true;
			}
			break;
			case irr::io::EXN_ELEMENT_END:
			{
				current_block = "";
			}
			break;
		}
	}
	xml->drop();
	return got_data;
}

bool BathyData::SaveToXMLFile(std::string FileName, irr::IrrlichtDevice * device)
{
	bool success = true;
	if (device == NULL)
		return false;
	irr::io::IXMLWriter* xwriter = device->getFileSystem()->createXMLWriter(irr::core::stringw(FileName.c_str()).c_str());
	if (!xwriter)
	{
		return false;
	}
	xwriter->writeXMLHeader();
	wchar_t tmp[100];
	swprintf(tmp,100,L"%d",depth_negative);
	xwriter->writeElement(L"depth_negative", true, L"dn",tmp);
	xwriter->writeLineBreak();                  //new line
	xwriter->writeElement(L"coordinates", true, 
							L"lat_min", str2wchar(std::to_string(lat_min)), 
							L"lat_max", str2wchar(std::to_string(lat_max)),
							L"lon_min", str2wchar(std::to_string(lon_min)),
							L"lon_max", str2wchar(std::to_string(lon_max)));
	xwriter->writeLineBreak();                  //new line
	xwriter->writeElement(L"Data");
	xwriter->writeLineBreak();                  //new line
	for (unsigned int x = 0;x< cell_sizes.getX(); x++)
	{
		for (unsigned int y = 0; y< cell_sizes.getY(); y++)
		{
			float depth = depths[y+x*cell_sizes.getY()];
			if (!depth_negative)
				depth *= -1.0f;
			wchar_t tmp[20];
			swprintf(tmp, 20, L"%f ", depth);
			xwriter->writeText(tmp);
		}
		xwriter->writeLineBreak();                  //new line
	}
	//unsigned int x = 0;
	//unsigned int y = 0;
	//float number;
	//std::string tmp;
	//while (std::getline(data, tmp, '\n'))
	//{
	//	y = 0;
	//	std::string tmp2;
	//	std::istringstream iss(tmp);
	//	while (std::getline(iss, tmp2, ' '))
	//	{
	//		number = (float)atof(tmp2.c_str());
	//		if (!depth_negative)
	//			number *= -1.0f;
	//		if (number > 0.0f)
	//			number = 0.0f;
	//		depths.push_back(number);
	//		y++;
	//		cell_sizes.setX(bm_max(cell_sizes.getX(), x));
	//		cell_sizes.setY(bm_max(cell_sizes.getY(), y));
	//	}
	//	x++;
	//}


	xwriter->writeClosingTag(L"Data");
	xwriter->writeLineBreak();                  //new line
	//delete xml writer
	xwriter->drop();
	return success;
}


bool BathyData::CreateEmpty(int X_cells, int Y_cells)
{
	depths.clear();
	cell_sizes.clear();
	cell_sizes.setX(X_cells);
	cell_sizes.setY(Y_cells);
	depths.clear();
	depths.resize(X_cells*Y_cells, 0.0f);
	return true;
}

void BathyData::reset_metadata()
{
	depth_negative = false;
	lat_max = 0.0f;
	lat_min = 0.0f;
	lon_max = 0.0f;
	lon_min = 0.0f;
}

void BathyData::AddRandomNoise(float delta)
{
	for (long i = 0; i < depths.size(); i++)
	{
		if (depths[i] < 0.0f)
		{
			depths[i] = bm_min(0,depths[i]+delta*(1.0f-2.0f*bm_rnd()));
		}
	}
}

int BathyData::SetDepth(int cell_no, float new_depth)
{
	if (cell_no >= 0 && cell_no < depths.size())
	{
		bm_undo tmp_undo;
		tmp_undo.cell_no = cell_no;
		tmp_undo.old_depth = depths[cell_no];
		depths[cell_no] = new_depth;
		tmp_undo.new_depth = new_depth;
		add_to_current_undo_step(tmp_undo);
		return true;
	}
	return false;
}

int BathyData::ChangeDepth(int cell_no, int range, float amount, bool land_mask_lock)
{
	return ChangeDepth(cell_no/cell_sizes.getY(), cell_no%cell_sizes.getY(),range,amount, land_mask_lock);
}

int BathyData::ChangeDepth(int x, int y, int range,float amount, bool land_mask_lock)
{
	if(x+range <0 || x-range > (signed int)cell_sizes.getX()-1 || y+range<0 || y-range>(signed int)cell_sizes.getY()-1)
		return 0; //No point is in the range of modification
	int modified = 0;
	for (int x_i = bm_max(0, x - range); x_i <(bm_min((signed int)cell_sizes.getX(),x + range)); x_i++)
	{
		for (int y_i = bm_max(0, y - range); y_i < (bm_min((signed int)cell_sizes.getY(), y + range)); y_i++)
		{
			float dist2 =(float)( pow(x_i - x, 2) + pow(y_i - y, 2));
			if (dist2 < range*range)
			{
				int c_ind = x_i*cell_sizes.getY() + y_i;
				if (!land_mask_lock || (depths[c_ind] < 0.0f && amount < 0.0f) || (depths[c_ind]<-2.0f*amount && amount>0.0f))
				{//Ensure no changes in land mask, if so wanted.
					float tmp = depths[c_ind];
					tmp += amount*(1.0f - (((float)sqrt(dist2)) / ((float)range)));
					tmp = bm_min(0.0f, tmp);
					SetDepth(c_ind, tmp);
					modified++;
				}
			}
		}
	}
	return modified;
}

int BathyData::ScaleDepths(int cell_no, bool land_mask_lock)
{
	if (depths[cell_no] == 0.0f)
		return false;
	int modified = 0;
	float scale_factor = fabs(depths[cell_no]);
	for (int i = 0; i < depths.size(); i++)
	{
		if (!land_mask_lock || depths[i] < 0.0f)
		{
			SetDepth(i, 100.0f*depths[i]/scale_factor);
//			std::cout << depths[cell_no] << " " << cell_no << " " << i << "\n";
			modified++;
		}
	}
	return modified;
}

int BathyData::SmoothDepth(int cell_no, int range, float amount, bool land_mask_lock)
{
	return SmoothDepth(cell_no / cell_sizes.getY(), cell_no%cell_sizes.getY(), range, amount, land_mask_lock);
}

int BathyData::SmoothDepth(int x, int y, int range, float amount, bool land_mask_lock)
{
	if (x + range <0 || x - range >(signed int)cell_sizes.getX() - 1 || y + range<0 || y - range>(signed int)cell_sizes.getY() - 1)
		return 0; //No point is in the range of modification
	//First calculate the average height:
	float avg_depth = 0.0f;
	float weight_total = 0.0f;
	for (int x_i = bm_max(0, x - range); x_i <(bm_min((signed int)cell_sizes.getX(), x + range)); x_i++)
	{
		for (int y_i = bm_max(0, y - range); y_i < (bm_min((signed int)cell_sizes.getY(), y + range)); y_i++)
		{
			float dist2 = (float)(pow(x_i - x, 2) + pow(y_i - y, 2));
			if (dist2 < range*range)
			{
				int c_ind = x_i*cell_sizes.getY() + y_i;
				if (!land_mask_lock || (depths[c_ind] < 0.0f && amount < 0.0f) || (depths[c_ind]<-2.0f*amount && amount>0.0f))
				{//Ensure no changes in land mask, if so wanted.
					avg_depth += depths[c_ind] * (1.0f - (((float)sqrt(dist2)) / ((float)range)));
					weight_total+= (1.0f - (((float)sqrt(dist2)) / ((float)range)));
				}
			}
		}
	}
	if (weight_total > 0.0f)
		avg_depth /= weight_total;
	else
		return 0;
	int modified = 0;
	//then modify the depths
	for (int x_i = bm_max(0, x - range); x_i <(bm_min((signed int)cell_sizes.getX(), x + range)); x_i++)
	{
		for (int y_i = bm_max(0, y - range); y_i < (bm_min((signed int)cell_sizes.getY(), y + range)); y_i++)
		{
			float dist2 = (float)(pow(x_i - x, 2) + pow(y_i - y, 2));
			if (dist2 < range*range)
			{
				int c_ind = x_i*cell_sizes.getY() + y_i;
				if (!land_mask_lock || (depths[c_ind] < 0.0f && amount < 0.0f) || (depths[c_ind]<-2.0f*amount && amount>0.0f))
				{//Ensure no changes in land mask, if so wanted.
					float diff = avg_depth-depths[c_ind];
					if (fabs(diff) > fabs(amount)) //difference is bigger than wanted smoothing
					{
						diff = -(diff*amount / (fabs(diff)));
					}
					//consider distance:
					diff *= (1.0f - (((float)sqrt(dist2)) / ((float)range)));
//					depths[c_ind] += diff;
					SetDepth(c_ind, depths[c_ind]+diff);
					modified++;
				}
			}
		}
	}
	return modified;
}

bool BathyData::add_to_current_undo_step(bm_undo new_entry)
{
	if (current_undo_step < -1)
		return false;
	if (current_undo_step >= (signed int)undo_steps.size())
		return false;
	if (undo_steps.size() == 0)
		open_undo_step();
	undo_steps[current_undo_step].push_back(new_entry);
	return true;
}

bool BathyData::open_undo_step()
{
	if (current_undo_step < -1 || current_undo_step >= (signed int)undo_steps.size())
		return false;
	if (undo_steps.size() == 0 || undo_steps[current_undo_step].size()>0)
	{//Now we need to create a new, empty undo step
		if (current_undo_step < (signed int)undo_steps.size() - 1 && undo_steps.size()>0)
		{//There are some undoed steps ahead, which will now be destroyed
			while(current_undo_step < (signed int)undo_steps.size() - 1 && undo_steps.size()>0)
				undo_steps.pop_back();  //remove the newer undoes.
		}
		std::vector<bm_undo> tmp;
		undo_steps.push_back(tmp); //pushes and empty, new list as a new undo step.
		current_undo_step++;
		return true;
	}
	return false;

}

bool BathyData::close_undo_step()
{
	open_undo_step(); //basically same thing than opening a new one.
	return false;
}

bool BathyData::make_undo()
{
	if(undo_steps.size()==0 || current_undo_step<1 || current_undo_step>(signed int)undo_steps.size()-1)
		return false;
	current_undo_step--;
	for (auto i = undo_steps[current_undo_step].begin(); i != undo_steps[current_undo_step].end(); i++)
	{
		depths[i->cell_no] = i->old_depth;
	}
	return true;
}

bool BathyData::make_redo()
{
	if (undo_steps.size() == 0 || current_undo_step<0 || current_undo_step>(signed int)undo_steps.size() - 2)
		return false;
	for (auto i = undo_steps[current_undo_step].begin(); i != undo_steps[current_undo_step].end(); i++)
	{
		depths[i->cell_no] = i->new_depth;
	}
	current_undo_step++;
	return false;
}

coordinate BathyData::cell2coordinate(int cell)
{
	coordinate result;
	return cell2coordinate(cell/cell_sizes.getY(), cell%cell_sizes.getY());
}

coordinate BathyData::cell2coordinate(int x, int y)
{
	coordinate result;
	result.x = x;
	result.y = y;
	if (lat_min >= lat_max || lon_min >= lon_max || cell_sizes.getY()<=0 || cell_sizes.getX() <= 0)
	{
		result.lat = lat_min;
		result.lon = lon_min;
	}
	else
	{
		result.lat = (((float)y)/((float)cell_sizes.getY()))*(lat_max-lat_min)+lat_min;
		result.lon = (((float)x) / ((float)cell_sizes.getX()))*(lon_max - lon_min) + lon_min;
	}
	return result;
}

