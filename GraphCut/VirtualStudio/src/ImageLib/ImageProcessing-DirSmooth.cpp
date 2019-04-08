#include "ImageProcessing.h"
#pragma warning(disable : 4101)
#pragma warning(disable : 4701)

void ImageProcessing::DirectionalSmoothing(CByteImage &image)
{
	ENSURE(image.Shape().nBands == 3);
	int m_Width  = image.Shape().width;
	int m_Height = image.Shape().height;

	int row, col, i, j, k, l, maxi, maxj, maxk, ct, minIdx, rd, cd, minIdx0, minIdx1, maxIdx;
	double r, g, b, denom, del, val, min, max, val0, val1, val2, val3;
	double diff[11];
	double diff2[20];
	double rgb[8][3];
	int rc[8][2];
	double rgb2[16][3];
	double finalRGB[3];
	int iter;
	double (*rgbSum)[3] = new double [m_Width*m_Height][3];
	double *rgbDenom = new double [m_Width*m_Height];
	
	for(iter=0;iter<1;iter++)
	{
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				rgbSum[row*m_Width + col][0] = 0.0;
				rgbSum[row*m_Width + col][1] = 0.0;
				rgbSum[row*m_Width + col][2] = 0.0;
				rgbDenom[row*m_Width + col] = 0.0;
			}
		}

		for(row=2;row<m_Height-2;row++)
		{

			for(col=2;col<m_Width-2;col++)
			{
				r = (double) image.Pixel(col, row, 0);
				g = (double) image.Pixel(col, row, 1);
				b = (double) image.Pixel(col, row, 2);

				for(i=0;i<8;i++)
				{
					switch(i)
					{
						case 0:
							rd = -1;
							cd = -1;
							break;
						case 1:
							rd = -1;
							cd = 0;
							break;
						case 2:
							rd = -1;
							cd = 1;
							break;
						case 3:
							rd = 0;
							cd = 1;
							break;
						case 4:
							rd = 1;
							cd = 1;
							break;
						case 5:
							rd = 1;
							cd = 0;
							break;
						case 6:
							rd = 1;
							cd = -1;
							break;
						case 7:
							rd = 0;
							cd = -1;
							break;
					}

					rc[i][0] = rd;
					rc[i][1] = cd;

					diff[i] = 0.0;
					diff[i] += (r - (double)  image.Pixel(col + cd, row + rd, 0)) * 
							   (r - (double)  image.Pixel(col + cd, row + rd, 0));
					diff[i] += (g - (double)  image.Pixel(col + cd, row + rd, 1)) * 
							   (g - (double)  image.Pixel(col + cd, row + rd, 1));
					diff[i] += (b - (double)  image.Pixel(col + cd, row + rd, 2)) * 
							   (b - (double)  image.Pixel(col + cd, row + rd, 2));

					rgb[i][0] = image.Pixel(col + cd, row + rd, 0);
					rgb[i][1] = image.Pixel(col + cd, row + rd, 1);
					rgb[i][2] = image.Pixel(col + cd, row + rd, 2);
				}

				for(i=0;i<16;i++)
				{
					switch(i)
					{
						case 0:
							rd = -2;
							cd = -2;
							break;
						case 1:
							rd = -2;
							cd = -1;
							break;
						case 2:
							rd = -2;
							cd = 0;
							break;
						case 3:
							rd = -2;
							cd = 1;
							break;
						case 4:
							rd = -2;
							cd = 2;
							break;
						case 5:
							rd = -1;
							cd = 2;
							break;
						case 6:
							rd = 0;
							cd = 2;
							break;
						case 7:
							rd = 1;
							cd = 2;
							break;
						case 8:
							rd = 2;
							cd = 2;
							break;
						case 9:
							rd = 2;
							cd = 1;
							break;
						case 10:
							rd = 2;
							cd = 0;
							break;
						case 11:
							rd = 2;
							cd = -1;
							break;
						case 12:
							rd = 2;
							cd = -2;
							break;
						case 13:
							rd = 1;
							cd = -2;
							break;
						case 14:
							rd = 0;
							cd = -2;
							break;
						case 15:
							rd = -1;
							cd = -2;
							break;
					}

					diff2[i] = 0.0;
					diff2[i] += (r - (double)  image.Pixel(col + cd, row + rd, 0)) * 
							    (r - (double)  image.Pixel(col + cd, row + rd, 0));
					diff2[i] += (g - (double)  image.Pixel(col + cd, row + rd, 1)) * 
							    (g - (double)  image.Pixel(col + cd, row + rd, 1));
					diff2[i] += (b - (double)  image.Pixel(col + cd, row + rd, 2)) * 
							    (b - (double)  image.Pixel(col + cd, row + rd, 2));

					rgb2[i][0] = image.Pixel(col + cd, row + rd, 0);
					rgb2[i][1] = image.Pixel(col + cd, row + rd, 1);
					rgb2[i][2] = image.Pixel(col + cd, row + rd, 2);
				}
				

				min = 9999999.0;
				max = 0.0;

				for(i=0;i<8;i++)
				{
					maxIdx = i;

					val = diff[i] + diff[(i-1+8)%8] + diff[(i+1)%8];
					val += diff2[i*2] + diff2[(i+2-1+16)%16] + diff2[(i*2-2+16)%16] + diff2[(i*2+1)%16] + diff2[(i*2+2)%16];

					if(val < min)
					{
						minIdx = i;
						min = val;
					}

					if(val > max)
					{
						max = val;
					}

				}


				rgbSum[row*m_Width + col][0] += r;
				rgbSum[row*m_Width + col][1] += g;
				rgbSum[row*m_Width + col][2] += b;
				rgbDenom[row*m_Width + col]++;

				for(i=0;i<3;i++)
				{
					for(j=0;j<3;j++)
					{
						rgbSum[row*m_Width + col][j] += rgb[(minIdx + i - 1 + 8)%8][j];
					}
					rgbDenom[row*m_Width + col]++;
				}
			}
		}

		for(row=2;row<m_Height-2;row++)
		{
			for(col=2;col<m_Width-2;col++)
			{
				rgbSum[row*m_Width + col][0] /= rgbDenom[row*m_Width + col];
				rgbSum[row*m_Width + col][1] /= rgbDenom[row*m_Width + col];
				rgbSum[row*m_Width + col][2] /= rgbDenom[row*m_Width + col];
			
				image.Pixel(col, row, 0) = ((uchar) rgbSum[row*m_Width + col][0]);
				image.Pixel(col, row, 1) = ((uchar) rgbSum[row*m_Width + col][1]);
				image.Pixel(col, row, 2) = ((uchar) rgbSum[row*m_Width + col][2]);
			}
		}
	}

	delete [] rgbSum;
	delete [] rgbDenom;
}

