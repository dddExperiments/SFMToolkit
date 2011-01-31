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
	if (argc < 6)
	{
		std::cout << "Usage: " << argv[0] << " <inputPath> <list.txt> <outfile matches> <matchThreshold> <firstOctave>" <<std::endl;
		std::cout << "<matchThreshold> : 0.0 means few match and 1.0 many match (float)" <<std::endl;
		std::cout << "<firstOctave>: specify on which octave start sampling (int)" <<std::endl;
		std::cout << "<firstOctave>: low value (0) means many features and high value (2) means less features" << std::endl;		
		std::cout << "Optional feature:" << std::endl;
		std::cout << "	- bin: generate binary files (needed for Augmented Reality tracking)" << std::endl;
		std::cout << "	- sequence NUMBER: matching optimized for video sequence" << std::endl;
		std::cout << "		-> example: sequence 3 (will match image N with N+1,N+2,N+3)" <<std::endl;
		std::cout << "Example: " << argv[0] << " your_folder/ list.txt gpu.matches.txt 0.8 1" << std::endl;

		return -1;
	}

	bool binnaryWritingEnabled  = false;
	bool sequenceMatching       = false;
	int  sequenceMatchingLength = 0;

	for (int i=1; i<argc; ++i)
	{
		std::string current(argv[i]);
		if (current == "bin")
			binnaryWritingEnabled = true;
		else if (current == "sequence")
		{
			if (i+1<argc)
			{				
				sequenceMatchingLength = atoi(argv[i+1]);
				if (sequenceMatchingLength > 0)
					sequenceMatching = true;
				i++;
			}
		}
	}

	BundlerMatcher matcher((float) atof(argv[4]), atoi(argv[5]), binnaryWritingEnabled, sequenceMatching, sequenceMatchingLength);
	matcher.open(std::string(argv[1]), std::string(argv[2]), std::string(argv[3]));
	
	return 0;
}
