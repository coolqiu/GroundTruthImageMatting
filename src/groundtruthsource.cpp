/**
* This application has the sole purpose of reading a series of images and processing them into the
* ground truth images, saving them to the same location with the given names
* */

#include "io.h"
#include <vector>
#include <opencv2/opencv.hpp>
#include "rawrgbchar.h"
#include "groundtruth.h"

/**
* Arguments:
* @arg The name of the program (default argument)
* @arg Colour1Path The filename of the foreground image with colour 1
* @arg Colour2Path The filename of the foreground image with colour 2
* @arg Colour3Path The filename of the foreground image with colour 3
* @arg Colour4Path The filename of the foreground image with colour 4
* @arg Colour5Path The filename of the foreground image with colour 5
* @arg Colourb1Path  The filename of the background image with colour 1
* @arg Colourb2Path  The filename of the background image with colour 2
* @arg Colourb3Path  The filename of the background image with colour 3
* @arg Colourb4Path  The filename of the background image with colour 4
* @arg Colourb5Path  The filename of the background image with colour 5
* @arg APath The name of the output alpha file
* @arg FPath The name of the output foreground file
* @arg AFPath The name of the output alpha-applied foreground file
*/

int main(int argc, char** argv)
{
	Inform("Entered Ground Truth generator");

	if(argc != 14)
	{
		Error("Invalid number of arguments: Expected 14, received " + ToString(argc));
		return 1;
	}

	//first five images are foregrounds, and the next five backgrounds
	Inform("Loading temporaries");
	RawRgbChar images[10];

	for (int i = 0; i < 10; ++i)
	{
		bool result = LoadRawRgb(argv[1 + i], images[i]);
		if (!result)
		{
			Error("Could not load " + ToString(argv[1 + i]));
			return 2;
		}
	}
	auto groundTruth = GenerateGroundTruth(&images[0], &images[5]);

	if (groundTruth.size() != 3)
		return 3;
	
	bool succeeded = true;
	for (size_t i = 0; i < 3; ++i)
	{
		const std::string path = (argv[11 + i]);
		Inform("Saving " + path);
		if (!cv::imwrite(path, groundTruth[i]))
		{
			Error("Could not save " + ToString(argv[11 + i]));
			succeeded = false;
		}
	}

	Inform("Exiting ground truth algorithm");
	return succeeded ? 0:4;
}
