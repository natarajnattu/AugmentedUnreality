/*
Copyright 2016 Krzysztof Lis

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http ://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "AugmentedUnreality.h"
#include "AURVideoSourceStream.h"

UAURVideoSourceStream::UAURVideoSourceStream()
	: ConnectionString("udpsrc port=5000 ! application/x-rtp, encoding-name=H264,payload=96 ! rtph264depay ! queue ! h264parse ! avdec_h264 ! videoconvert ! appsink")
{
}

FText UAURVideoSourceStream::GetSourceName() const
{
	if(!SourceName.IsEmpty())
	{
		return SourceName;
	}
	else
	{
		if (!ConnectionString.IsEmpty())
		{
			return FText::FromString("GStreamer");
		}
		else 
		{
			return FText::FromString("Stream " + FPaths::GetCleanFilename(StreamFile));
		}
	}
}

bool UAURVideoSourceStream::Connect()
{
	bool success = false;

	try
	{
		if (!ConnectionString.IsEmpty())
		{
			success = OpenVideoCapture(ConnectionString);
		}
		else
		{
			FString full_path = FPaths::GameDir() / StreamFile;

			if (!FPaths::FileExists(full_path))
			{
				UE_LOG(LogAUR, Error, TEXT("UAURVideoSourceStream::Connect: File %s does not exist"), *full_path)
					return false;
			}

			success = OpenVideoCapture(full_path);
		}

		if (success)
		{
			Capture.set(cv::CAP_PROP_BUFFERSIZE, 1);
			LoadCalibration();
		}

		return Capture.isOpened();
	}
	catch (std::exception& exc)
	{
		UE_LOG(LogAUR, Error, TEXT("UAURVideoSourceStream::Connect: exception \n    %s"), UTF8_TO_TCHAR(exc.what()))
	}

	return false;
}
