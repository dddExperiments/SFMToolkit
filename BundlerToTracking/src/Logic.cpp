#include "Logic.h"

#include <fstream>
#include <sstream>
#include <iostream>

#include <IL/il.h>

Logic::Logic()
{
	//DevIL init
	ilInit();
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	ilEnable(IL_ORIGIN_SET);
}

Logic::~Logic()
{
	//DevIL shutdown
	ilShutDown();
}

void Logic::open(const std::string& filename)
{
	parseListFile(filename);
	
	std::stringstream command;
	for (unsigned int i=0; i<mFilenames.size(); ++i)
	{
		std::stringstream keyPointFilename;
		keyPointFilename << mFilenames[i] << ".key";
		if (!existFile(keyPointFilename.str()))
		{
			std::cout << "-------------------------------------------------------------------------------" <<std::endl;
			std::cout << "--> Extracting feature from image " << (i+1) << "/" << (mFilenames.size()) << std::endl;
			std::cout << "-------------------------------------------------------------------------------" <<std::endl;	

			//convert .jpg .pgm
			command << ".\\bin32\\nconvert.exe -grey 256 -o " << mFilenames[i] << ".pgm -out pgm "<<mFilenames[i];		
			system(command.str().c_str());
			
			command.str("");

			//run siftwin32 pgm .key
			command << ".\\bin32\\siftWin32.exe < " << mFilenames[i] << ".pgm > " << mFilenames[i] << ".key";
			system(command.str().c_str());

			command.str("");

			//delete .pgm
			command << "del \"" << mFilenames[i] << ".pgm\" /Q";
			system(command.str().c_str());

			command.str("");
		}
	}

	saveFeatures();

	//create TMP_listKey.txt
	createListKey();

	//match using cpu
	command.str(".\\bin32\\KeyMatchFull.exe TMP_listKey.txt cpu.matches.txt");
	system(command.str().c_str());

	//delete TMP_listKey.txt
	system("del TMP_listKey.txt /Q");
}

bool Logic::parseListFile(const std::string& filename)
{
	std::ifstream input(filename.c_str());
	if (input.is_open())
	{
		while(!input.eof())
		{
			std::string line;
			std::getline(input, line);
			if (line != "")
				mFilenames.push_back(line);
		}
	}
	input.close();

	return true;
}

void Logic::createListKey()
{
	std::ofstream output;
	output.open("TMP_listKey.txt");
	if (output.is_open())
	{
		for (unsigned int i=0; i<mFilenames.size(); ++i)
			output << mFilenames[i] << ".key" << std::endl;
		output << std::endl;
	}
	output.close();
}

bool Logic::getDim(const std::string& filename, int& width, int& height)
{
	bool loaded = false;
	unsigned int imgId = 0;
	ilGenImages(1, &imgId);
	ilBindImage(imgId); 

	if(ilLoadImage(filename.c_str()))
	{
		width  = ilGetInteger(IL_IMAGE_WIDTH);
		height = ilGetInteger(IL_IMAGE_HEIGHT);
		loaded = true;
	}
	ilDeleteImages(1, &imgId); 

	return loaded;
}

void Logic::saveFeatures()
{
	std::ofstream output;
	output.open("cpu.features.bin", std::ios::out | std::ios::binary);
	for (unsigned int i=0; i<mFilenames.size(); ++i)
	{
		//open jpeg to get dimensions
		int width, height = 0;
		getDim(mFilenames[i], width, height);
		output.write((char*)&width, sizeof(width));
		output.write((char*)&height, sizeof(height));

		//open key file
		std::stringstream filename;
		filename << mFilenames[i] << ".key";
		std::vector<Feature> features = getFeatures(filename.str());
		
		int nbFeature = (int)features.size();
		output.write((char*)&nbFeature, sizeof(nbFeature));
		for (int j=0; j<nbFeature; ++j)
		{
			float x = features[j].x;
			float y = features[j].y;

			output.write((char*)&x, sizeof(x));
			output.write((char*)&y, sizeof(y));
		}
	}
	output.close();
}

std::vector<Feature> Logic::getFeatures(const std::string& filename)
{
	std::vector<Feature> features;

	std::ifstream input;
	input.open(filename.c_str());
	if (input.is_open())
	{
		int nbFeature;
		int descriptorSize;
		input >> nbFeature;
		input >> descriptorSize;
		for (int i=0; i<nbFeature; ++i)
		{
			float x, y, scale, orientation;
			input >> x;
			input >> y;
			input >> scale;
			input >> orientation;
			features.push_back(Feature(x, y));
			for (int j=0; j<descriptorSize; ++j)
			{
				int descriptor;
				input >> descriptor;
			}
		}
	}
	input.close();

	return features;
}

bool Logic::existFile(const std::string& filename)
{
	std::ifstream input;
	input.open(filename.c_str());	
	bool exist = input.is_open();
	input.close();

	return exist;
}