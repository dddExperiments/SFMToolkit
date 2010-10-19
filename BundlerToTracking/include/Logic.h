#pragma once

#include <string>
#include <vector>

struct Feature
{
	Feature(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
	float x;
	float y;
};

class Logic
{
	public:
		Logic();
		~Logic();

		void open(const std::string& filename);

	protected:
		std::vector<Feature> getFeatures(const std::string& filename);
		bool getDim(const std::string& filename, int& width, int& height);
		bool parseListFile(const std::string& filename);
		bool existFile(const std::string& filename);
		void createListKey();
		void saveFeatures();

		std::vector<std::string> mFilenames;
};