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

#include "BundlerMatcher.h"

int main(int argc, char* argv[])
{
	if (argc < 5)
	{
		std::cout << "Usage: " << argv[0] << " <list.txt> <outfile matches> <matchThreshold> <firstOctave>" <<std::endl;
		std::cout << "<matchThreshold> : 0.0 means few match and 1.0 many match (float)" <<std::endl;
		std::cout << "<firstOctave>: specify on which octave start sampling (int)" <<std::endl;
		std::cout << "<firstOctave>: low value (0) means many features and high value (2) means less features" << std::endl;		
		std::cout << "Example: " << argv[0] << " list.txt gpu.matches.txt 0.8 1" << std::endl;

		return -1;
	}

	bool binnaryWritingEnabled = false;

	for (int i=1; i<argc; ++i)
	{
		std::string current(argv[i]);
		if (current == "bin")
			binnaryWritingEnabled = true;
	}

	BundlerMatcher matcher((float) atof(argv[3]), atoi(argv[4]), binnaryWritingEnabled);
	matcher.open(std::string(argv[1]), std::string(argv[2]));

	return 0;
}
