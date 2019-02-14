#include "Camera.h"
#include "ImageIO.h"

CShape Camera::GetFrameShape(int frame, float scale) const
{
	CHECK_RANGE(frame, 0, this->frameCount - 1);
	string frameName = GetFrameName(frame);
	return ImageIO::GetImageShape(frameName, scale);
}
