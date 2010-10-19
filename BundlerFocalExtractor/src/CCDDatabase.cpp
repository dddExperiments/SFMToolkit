/*
	Copyright (c) 2010 ASTRE Henri (http://www.visual-experiments.com)

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#include <tinyxml.h>

#include "CCDDatabase.h"

CCDDatabaseManager::CCDDatabaseManager(const std::string& filepath)
{
	if (!fillCCDWidthDatabase(filepath))
	{
		std::cerr << "CCD Database empty: " << filepath << " is missing" << std::endl;
	}
}

float CCDDatabaseManager::getCCDWidth(const std::string& make, const std::string& model) const
{
	std::stringstream id;
	id << make << " " << model;

	std::map<std::string, float>::const_iterator it = mCCDWidthDatabase.find(id.str());

	if (it != mCCDWidthDatabase.end())
		return it->second;
	else
		return -1;
}

bool CCDDatabaseManager::fillCCDWidthDatabase(const std::string& filepath)
{
	TiXmlDocument doc;
	if (doc.LoadFile(filepath))
	{
		TiXmlNode* root = doc.FirstChild("cameras");
		if (root)
		{
			for (TiXmlNode* node = root->FirstChild(); node; node = node->NextSibling())
			{
				//ignore comment
				if (node->Type() == TiXmlNode::ELEMENT)
				{		
					//add cameras to the database
					if (std::string(node->ToElement()->Value()) == "camera")
					{
						std::string model = node->ToElement()->Attribute("model");
						float ccdWidth    = (float) atof(node->ToElement()->Attribute("ccdWidth"));

						mCCDWidthDatabase[model] = ccdWidth;
					}
				}
			}
		}
		return true;
	}
	else
		return false;
}