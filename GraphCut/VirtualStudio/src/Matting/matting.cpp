#include "stdafx.h"
using namespace vt;
#include "StereoSegment.h"


#define	MATTING_RAD						3

/******************************************************************************************
First call:

	findDiscontinuities

Call the following until convergence:

	computeFBColors(pDC);
	computeAlpha(pDC);
	combineAlphas();

*******************************************************************************************/

void CStereoSegmentDoc::findDiscontinuities()
{
	int iCamera, row, col, rd, cd, i, k, max, dist;
	RGBAPix *pixSrc;
	float dis, ct0, ct1, disBack, val;
	float dis1[MATTING_RAD*MATTING_RAD];
	int disCt[MATTING_RAD*MATTING_RAD];
	bool found;

	m_MatDisparity = new float [m_Width*m_Height][MAX_NUM_CAMERAS][2]; //disparity for foreground and background
	m_MatColor = new float [m_Width*m_Height][MAX_NUM_CAMERAS][2][3]; //f and g color
	m_Alpha = new float [m_Width*m_Height][MAX_NUM_CAMERAS][3]; //final alpha
	m_MatDist = new int [m_Width*m_Height][MAX_NUM_CAMERAS]; //
	m_MatFix = new int [m_Width*m_Height][MAX_NUM_CAMERAS]; //
//	m_MaxAlpha = new float [m_Width*m_Height][MAX_NUM_CAMERAS];

	for(iCamera=0;iCamera<m_NumCameras;iCamera++)
	{

		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				m_MatDisparity[row*m_Width + col][iCamera][0] = (float)m_Disparity[row*m_Width + col][iCamera];
				m_MatDisparity[row*m_Width + col][iCamera][1] = NO_INFORMATION;

				m_MatColor[row*m_Width + col][iCamera][0][0] = 0.0;
				m_MatColor[row*m_Width + col][iCamera][0][1] = 0.0;
				m_MatColor[row*m_Width + col][iCamera][0][2] = 0.0;
				m_MatColor[row*m_Width + col][iCamera][1][0] = 0.0;
				m_MatColor[row*m_Width + col][iCamera][1][1] = 0.0;
				m_MatColor[row*m_Width + col][iCamera][1][2] = 0.0;

				m_Alpha[row*m_Width + col][iCamera][0] = 0.0;
				m_Alpha[row*m_Width + col][iCamera][1] = 0.0;
				m_Alpha[row*m_Width + col][iCamera][2] = 0.0;

				m_MatDist[row*m_Width + col][iCamera] = 0;
				m_MatFix[row*m_Width + col][iCamera] = 0;

				dis = (float)m_Disparity[row*m_Width + col][iCamera]; //curr pixel disparity

				ct0 = 0.0;
				ct1 = 0.0;

				memset(disCt, 0, MATTING_RAD*MATTING_RAD*sizeof(int));	 //disct hist

				for(rd=-MATTING_RAD;rd<=MATTING_RAD;rd++)
				{
					for(cd=-MATTING_RAD;cd<=MATTING_RAD;cd++)
					{
						if(row + rd >= 0 && row + rd < m_Height &&
							col + cd >= 0 && col + cd < m_Width)
						{
							if(fabs(dis - m_Disparity[(row + rd)*m_Width + col + cd][iCamera]) > SAME_SURFACE)
							{
								found = false;
								for(i=0;!found && i<MATTING_RAD*MATTING_RAD;i++)
								{
									if(disCt[i] == 0)
									{
										dis1[i] = (float)m_Disparity[(row + rd)*m_Width + col + cd][iCamera];
										disCt[i]++;
										found = true;
									}
									else
									{
										if(fabs(dis1[i] - m_Disparity[(row + rd)*m_Width + col + cd][iCamera]) <= SAME_SURFACE)
										{
											disCt[i]++;
											found = true;
										}
									}
								}
							}
						}

					}
				}

				max = 0;
				for(i=0;i<MATTING_RAD*MATTING_RAD;i++)
				{
					if(max < disCt[i])
					{
						max = disCt[i];
						disBack = dis1[i];
					}
				}



				for(rd=-MATTING_RAD;rd<=MATTING_RAD;rd++)
				{
					for(cd=-MATTING_RAD;cd<=MATTING_RAD;cd++)
					{
						if(row + rd >= 0 && row + rd < m_Height &&
							col + cd >= 0 && col + cd < m_Width)
						{
							pixSrc = m_Image[iCamera].Ptr(col + cd,row + rd);

							if(fabs(dis - m_Disparity[(row + rd)*m_Width + col + cd][iCamera]) <= SAME_SURFACE)
							{
								if(0)//idx0 == seg->m_SegmentIdx[(row + rd)*m_Width + col + cd])
								{
									ct0++;

									m_MatColor[row*m_Width + col][iCamera][0][0] += float (pixSrc->r);
									m_MatColor[row*m_Width + col][iCamera][0][1] += float (pixSrc->g);
									m_MatColor[row*m_Width + col][iCamera][0][2] += float (pixSrc->b);
								}
							}
							else
							{
								if(fabs(disBack - m_Disparity[(row + rd)*m_Width + col + cd][iCamera]) <= SAME_SURFACE)
								{
									m_MatDisparity[row*m_Width + col][iCamera][1] = 
										(float)((ct1*m_MatDisparity[row*m_Width + col][iCamera][1] + m_Disparity[(row + rd)*m_Width + col + cd][iCamera])/(ct1 + 1.0));
								
									ct1++;

									m_MatColor[row*m_Width + col][iCamera][1][0] += float (pixSrc->r);
									m_MatColor[row*m_Width + col][iCamera][1][1] += float (pixSrc->g);
									m_MatColor[row*m_Width + col][iCamera][1][2] += float (pixSrc->b);

								}
							}
						}
					}
				}
				
				pixSrc = m_Image[iCamera].Ptr(col,row);
				m_MatColor[row*m_Width + col][iCamera][0][0] = float (pixSrc->r); //0 main color, 1 secondary color
				m_MatColor[row*m_Width + col][iCamera][0][1] = float (pixSrc->g);
				m_MatColor[row*m_Width + col][iCamera][0][2] = float (pixSrc->b);

			//	m_MatColor[row*m_Width + col][iCamera][0][0] /= ct0;
			//	m_MatColor[row*m_Width + col][iCamera][0][1] /= ct0;
			//	m_MatColor[row*m_Width + col][iCamera][0][2] /= ct0;
				m_MatColor[row*m_Width + col][iCamera][1][0] /= ct1;
				m_MatColor[row*m_Width + col][iCamera][1][1] /= ct1;
				m_MatColor[row*m_Width + col][iCamera][1][2] /= ct1;

				if(m_MatDisparity[row*m_Width + col][iCamera][1] == NO_INFORMATION)
				{
					pixSrc = m_Image[iCamera].Ptr(col,row);

					m_MatColor[row*m_Width + col][iCamera][0][0] = (float) pixSrc->r;
					m_MatColor[row*m_Width + col][iCamera][0][1] = (float) pixSrc->g;
					m_MatColor[row*m_Width + col][iCamera][0][2] = (float) pixSrc->b;
				}

			}
		}
	}

	// make sure [0] is the background
	for(iCamera=0;iCamera<m_NumCameras;iCamera++)
	{
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				if(m_MatDisparity[row*m_Width + col][iCamera][1] != NO_INFORMATION)
				{
					if(m_MatDisparity[row*m_Width + col][iCamera][0] < 
						m_MatDisparity[row*m_Width + col][iCamera][1])
					{
						for(k=0;k<3;k++)
						{
							val = m_MatColor[row*m_Width + col][iCamera][0][k];
							m_MatColor[row*m_Width + col][iCamera][0][k] = m_MatColor[row*m_Width + col][iCamera][1][k];
							m_MatColor[row*m_Width + col][iCamera][1][k] = val;
						}

						val = m_MatDisparity[row*m_Width + col][iCamera][0];
						m_MatDisparity[row*m_Width + col][iCamera][0] = m_MatDisparity[row*m_Width + col][iCamera][1];
						m_MatDisparity[row*m_Width + col][iCamera][1] = val;

						m_Alpha[row*m_Width + col][iCamera][0] = 1.0f - m_Alpha[row*m_Width + col][iCamera][0];
						m_Alpha[row*m_Width + col][iCamera][1] = 1.0f - m_Alpha[row*m_Width + col][iCamera][1];
						m_Alpha[row*m_Width + col][iCamera][2] = 1.0f - m_Alpha[row*m_Width + col][iCamera][2];

					}
				}
			}
		}
	}


	for(iCamera=0;iCamera<m_NumCameras;iCamera++)
	{
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{


				if(m_MatDisparity[row*m_Width + col][iCamera][1] != NO_INFORMATION)
				{
					dis = m_MatDisparity[row*m_Width + col][iCamera][0];
					m_MatDist[row*m_Width + col][iCamera] = 100;

					for(rd=-3*MATTING_RAD;rd<=3*MATTING_RAD;rd++)
					{
						for(cd=-3*MATTING_RAD;cd<=3*MATTING_RAD;cd++)
						{
							if(row + rd >= 0 && row + rd < m_Height &&
								col + cd >= 0 && col + cd < m_Width)
							{
								if((fabs(dis - m_MatDisparity[(row + rd)*m_Width + col + cd][iCamera][0]) <= SAME_SURFACE &&
									m_MatDisparity[(row + rd)*m_Width + col + cd][iCamera][1] == NO_INFORMATION) ||
									(fabs(dis - m_MatDisparity[(row + rd)*m_Width + col + cd][iCamera][1]) <= SAME_SURFACE))
								{
									dist = (int) sqrt((double) (rd*rd) + (double) (cd*cd)) + 1;
									if(dist < m_MatDist[row*m_Width + col][iCamera])
									{
										m_MatDist[row*m_Width + col][iCamera] = dist;
										for(k=0;k<3;k++)
										{
											m_Alpha[row*m_Width + col][iCamera][k] = 
												(float)(min(1.0, max(0.0, (double) dist/(2.0*MATTING_RAD) - 0.25)));
										}
									}
								}
							}
						}
					}

					
				}
			}
		}

		int *map = new int [m_Width*m_Height];
		
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				map[row*m_Width + col] = 0;

				if(m_MatDist[row*m_Width + col][iCamera] == 100)
				{
					map[row*m_Width + col] = 1;
				}
			}
		}

		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				if(m_MatDist[row*m_Width + col][iCamera] == 100)
				{
					for(rd=-3*MATTING_RAD;rd<=3*MATTING_RAD;rd++)
					{
						for(cd=-3*MATTING_RAD;cd<=3*MATTING_RAD;cd++)
						{
							if(row + rd >= 0 && row + rd < m_Height &&
								col + cd >= 0 && col + cd < m_Width)
							{
								if(map[(row + rd)*m_Width + col + cd] == 0)
								{
									dist = (int) sqrt((double) (rd*rd) + (double) (cd*cd)) + 1;
								//	dist = 2*MATTING_RAD + 2 - dist;
									if(dist < m_MatDist[row*m_Width + col][iCamera])
									{
										m_MatDist[row*m_Width + col][iCamera] = dist;
										for(k=0;k<3;k++)
										{
											m_Alpha[row*m_Width + col][iCamera][k] = 
												(float)(min(1.0, max(0.0, (double) dist/(2.0*MATTING_RAD) - 0.25)));
										}
									}
								}
							}
						}
					}

				}
			}
		}

		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				if(map[row*m_Width + col] == 1)
				{
					m_MatDist[row*m_Width + col][iCamera] = 2*MATTING_RAD + 2 - m_MatDist[row*m_Width + col][iCamera]; 
				}
			}
		}

		delete [] map;
	}
	


	for(iCamera=0;iCamera<m_NumCameras;iCamera++)
	{
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				pixSrc = m_SmoothImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) m_MatColor[row*m_Width + col][iCamera][0][0]);
				pixSrc->g = ((BYTE) m_MatColor[row*m_Width + col][iCamera][0][1]);
				pixSrc->b = ((BYTE) m_MatColor[row*m_Width + col][iCamera][0][2]);

				pixSrc = m_SegmentImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) m_MatColor[row*m_Width + col][iCamera][1][0]);
				pixSrc->g = ((BYTE) m_MatColor[row*m_Width + col][iCamera][1][1]);
				pixSrc->b = ((BYTE) m_MatColor[row*m_Width + col][iCamera][1][2]);

				pixSrc = m_DisparityImage[iCamera].Ptr(col,row);

				dist = m_MatDist[row*m_Width + col][iCamera];
				pixSrc->r = ((BYTE) 30*dist);
				pixSrc->g = ((BYTE) 30*dist);
				pixSrc->b = ((BYTE) 30*dist);


				pixSrc = m_SmoothImage[iCamera].Ptr(col,row);
				val = m_MatDisparity[row*m_Width + col][iCamera][0]/m_NumZPlanes;
				val *= 255.0;

				pixSrc->r = ((BYTE) val);
				pixSrc->g = ((BYTE) val);
				pixSrc->b = ((BYTE) val);

				pixSrc = m_SegmentImage[iCamera].Ptr(col,row);
				val = m_MatDisparity[row*m_Width + col][iCamera][1]/m_NumZPlanes;
				val *= 255.0;

				pixSrc->r = ((BYTE) val);
				pixSrc->g = ((BYTE) val);
				pixSrc->b = ((BYTE) val);
			}
		}
	}

} // findDiscontinuities

void CStereoSegmentDoc::computeFBColors(CDC *pDC)
{
	int iCamera, row, col, rd, cd, k;
	RGBAPix *pixSrc;
	float dis0, dis1, denom, ct0[3], ct1[3], alpha[3], color0[3], color1[3], rgb[3], mat[2][2], del;
	float (*tmpFColor)[3] = new float [m_Width*m_Height][3];
	float (*tmpBColor)[3] = new float [m_Width*m_Height][3];


	for(iCamera=0;iCamera<m_NumCameras;iCamera++)
	{
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			{
				for(k=0;k<3;k++)
				{
					tmpFColor[row*m_Width + col][k] = m_MatColor[row*m_Width + col][iCamera][1][k];
					tmpBColor[row*m_Width + col][k] = m_MatColor[row*m_Width + col][iCamera][0][k];
				}
			}
		}				
		
		for(row=0;row<m_Height;row++)
		{
			for(col=0;col<m_Width;col++)
			if(m_MatDisparity[row*m_Width + col][iCamera][1] != NO_INFORMATION)
			{
				dis0 = m_MatDisparity[row*m_Width + col][iCamera][0];
				dis1 = m_MatDisparity[row*m_Width + col][iCamera][1];

				for(k=0;k<3;k++)
				{
					ct0[k] = 0.0;
					ct1[k] = 0.0;
					color0[k] = 0.0;//m_MatColor[row*m_Width + col][iCamera][0][k];
					color1[k] = 0.0;
				//	m_MatColor[row*m_Width + col][iCamera][0][k] = 0.0;
				//	m_MatColor[row*m_Width + col][iCamera][1][k] = 0.0;
				}

				for(rd=-MATTING_RAD;rd<=MATTING_RAD;rd++)
				{
					for(cd=-MATTING_RAD;cd<=MATTING_RAD;cd++)
					{
						if(row + rd >= 0 && row + rd < m_Height &&
							col + cd >= 0 && col + cd < m_Width)
						{
						//	del = 1.0/(fabs((double) cd) + fabs((double) rd) + 1.0);

							del = 1.0f;

							for(k=0;k<3;k++)
							{
								alpha[k] = m_Alpha[(row + rd)*m_Width + col + cd][iCamera][k];
								alpha[k] = min(0.999f, max(0.001f, alpha[k]));
							}

							if(fabs(dis0 - m_MatDisparity[(row + rd)*m_Width + col + cd][iCamera][0]) <= SAME_SURFACE)
							{
								for(k=0;k<3;k++)
								{
									color0[k] +=
										del*(1.0f - alpha[k])*(1.0f - alpha[k])*tmpBColor[(row + rd)*m_Width + col + cd][k];

									ct0[k] += del*(1.0f - alpha[k])*(1.0f - alpha[k]);
								}
							}

							if(fabs(dis1 - m_MatDisparity[(row + rd)*m_Width + col + cd][iCamera][1]) <= SAME_SURFACE)
							{	
								for(k=0;k<3;k++)
								{
									color1[k] += 
										del*alpha[k]*alpha[k]*tmpFColor[(row + rd)*m_Width + col + cd][k];
									ct1[k] += del*alpha[k]*alpha[k];
								}
							}
							if(fabs(dis1 - m_MatDisparity[(row + rd)*m_Width + col + cd][iCamera][0]) <= SAME_SURFACE)
							{	
								for(k=0;k<3;k++)
								{
									color1[k] += 
										del*(1.0f - alpha[k])*(1.0f - alpha[k])*tmpBColor[(row + rd)*m_Width + col + cd][k];
									ct1[k] += del*(1.0f - alpha[k])*(1.0f - alpha[k]);
								}
							}
						}
					}
				}
							
				pixSrc = m_Image[iCamera].Ptr(col,row);

				rgb[0] = (float) pixSrc->r;
				rgb[1] = (float) pixSrc->g;
				rgb[2] = (float) pixSrc->b;
				
				for(k=0;k<3;k++)
				{
					alpha[k] = m_Alpha[(row)*m_Width + col][iCamera][k];
					alpha[k] = min(0.999f, max(0.001f, alpha[k]));

					color0[k] /= ct0[k];
					color1[k] /= ct1[k];


				//	color0[k] += rgb[k]*(1.0 - alpha[k]);
				//	color1[k] += rgb[k]*(alpha[k]);

					denom = (1.0f + (1.0f - alpha[k])*(1.0f - alpha[k]))*(1.0f + alpha[k]*alpha[k]) - 
						(alpha[k]*(1.0f - alpha[k]))*(alpha[k]*(1.0f - alpha[k]));

					if(denom == 0.0f)
					{
						pixSrc = m_DisparityImage[iCamera].Ptr(col,row);
						pixSrc->r = (255);
						pixSrc->g = (0);
						pixSrc->b = (0);
					}

					mat[0][0] = (1.0f + alpha[k]*alpha[k])/denom;
					mat[0][1] = -(alpha[k]*(1.0f - alpha[k]))/denom;
					mat[1][0] = -(alpha[k]*(1.0f - alpha[k]))/denom;
					mat[1][1] = (1.0f + (1.0f - alpha[k])*(1.0f - alpha[k]))/denom;

				//	m_MatColor[row*m_Width + col][iCamera][1][k] = mat[1][0]*color0[k] + mat[1][1]*color1[k];
				//	m_MatColor[row*m_Width + col][iCamera][1][k] = min(255.0, max(0.0, m_MatColor[row*m_Width + col][iCamera][1][k]));
					m_MatColor[row*m_Width + col][iCamera][1][k] = color1[k];
					m_MatColor[row*m_Width + col][iCamera][1][k] = min(255.0f, max(0.0f, m_MatColor[row*m_Width + col][iCamera][1][k]));

				//	m_MatColor[row*m_Width + col][iCamera][0][k] = mat[0][0]*color0[k] + mat[0][1]*color1[k];
				//	m_MatColor[row*m_Width + col][iCamera][0][k] = min(255.0, max(0.0, m_MatColor[row*m_Width + col][iCamera][0][k]));
					m_MatColor[row*m_Width + col][iCamera][0][k] = color0[k];
					m_MatColor[row*m_Width + col][iCamera][0][k] = min(255.0f, max(0.0f, m_MatColor[row*m_Width + col][iCamera][0][k]));
				}
			}
		}
	}

	
	for(iCamera=0;iCamera<m_NumCameras;iCamera++) {
		for(row=0;row<m_Height;row++) {
            int indrow = row*m_Width;

			for(col=0;col<m_Width;col++) {
                int index = indrow + col;

				pixSrc = m_SmoothImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) m_MatColor[index][iCamera][0][0]);
				pixSrc->g = ((BYTE) m_MatColor[index][iCamera][0][1]);
				pixSrc->b = ((BYTE) m_MatColor[index][iCamera][0][2]);

				pixSrc = m_DisparityImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) m_MatColor[index][iCamera][1][0]);
				pixSrc->g = ((BYTE) m_MatColor[index][iCamera][1][1]);
				pixSrc->b = ((BYTE) m_MatColor[index][iCamera][1][2]);
			}
		}
        DisplayInHdc(pDC, m_DisparityImage[iCamera], 0, 0);
	}

	delete [] tmpFColor;
	delete [] tmpBColor;

} // computeFBColors

void CStereoSegmentDoc::computeAlpha(CDC *pDC)
{
	int iCamera, row, col;
	RGBAPix *pixSrc;
	float r, g, b, rgb[3];

	for(iCamera=0;iCamera<m_NumCameras;iCamera++) {
		for(row=0;row<m_Height;row++) {
            int indrow = row*m_Width;

			for(col=0;col<m_Width;col++) {
                int index = indrow + col;

				if(m_MatDisparity[index][iCamera][1] != NO_INFORMATION)
				{
					pixSrc = m_Image[iCamera].Ptr(col,row);

					r = (float) pixSrc->r;
					g = (float) pixSrc->g;
					b = (float) pixSrc->b;

					rgb[0] = r;
					rgb[1] = g;
					rgb[2] = b;
				
					m_Alpha[index][iCamera][0] = 0.0;
					m_Alpha[index][iCamera][1] = 0.0;
					m_Alpha[index][iCamera][2] = 0.0;

					if(fabs(m_MatColor[index][iCamera][1][0] - m_MatColor[index][iCamera][0][0]) > 5.0)
					{
						m_Alpha[index][iCamera][0] = (r - m_MatColor[index][iCamera][0][0])/
							(m_MatColor[index][iCamera][1][0] - m_MatColor[index][iCamera][0][0]);
					}

					if(fabs(m_MatColor[index][iCamera][1][1] - m_MatColor[index][iCamera][0][1]) > 5.0)
					{
						m_Alpha[index][iCamera][1] = (g - m_MatColor[index][iCamera][0][1])/
							(m_MatColor[index][iCamera][1][1] - m_MatColor[index][iCamera][0][1]);
					}

					if(fabs(m_MatColor[index][iCamera][1][2] - m_MatColor[index][iCamera][0][2]) > 5.0)
					{
						m_Alpha[index][iCamera][2] = (b - m_MatColor[index][iCamera][0][2])/
							(m_MatColor[index][iCamera][1][2] - m_MatColor[index][iCamera][0][2]);
					}

					m_Alpha[index][iCamera][0] = min(1.0f, max(0.0f, m_Alpha[index][iCamera][0]));
					m_Alpha[index][iCamera][1] = min(1.0f, max(0.0f, m_Alpha[index][iCamera][1]));
					m_Alpha[index][iCamera][2] = min(1.0f, max(0.0f, m_Alpha[index][iCamera][2]));
				}
			}
		}
	}

	for(iCamera=0;iCamera<m_NumCameras;iCamera++) {
		for(row=0;row<m_Height;row++) {
            int indrow = row*m_Width;

			for(col=0;col<m_Width;col++) {
                int index = indrow + col;

				pixSrc = m_SegmentImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) (255.0*m_Alpha[index][iCamera][0]));
				pixSrc->g = ((BYTE) (255.0*m_Alpha[index][iCamera][1]));
				pixSrc->b = ((BYTE) (255.0*m_Alpha[index][iCamera][2]));
			}
		}

        DisplayInHdc(pDC, m_SegmentImage[iCamera], 0, 0);
	}
} // computeAlphas


void CStereoSegmentDoc::combineAlphas()
{
	int iCamera, row, col;
	RGBAPix *pixSrc;
	float r, g, b, ra, ga, ba, error, min, alpha, minAlpha;

	for(iCamera=0;iCamera<m_NumCameras;iCamera++) {
		for(row=0;row<m_Height;row++) {
            int indrow = row*m_Width;

			for(col=0;col<m_Width;col++) {
                int index = indrow + col;

				if(m_MatDisparity[index][iCamera][1] != NO_INFORMATION)
				{
					pixSrc = m_Image[iCamera].Ptr(col,row);
					r = (float) pixSrc->r;
					g = (float) pixSrc->g;
					b = (float) pixSrc->b;

					min = 9999999999.0f;
					for( alpha = 0.0; alpha <= 1.01; alpha += 0.01f )
					{
					//	error = alpha*30.0;
						error = 0.0f;

						ra = alpha*m_MatColor[index][iCamera][1][0] + 
							(1.0f - alpha)*m_MatColor[index][iCamera][0][0];
						error += fabs(ra - r);

						ga = alpha*m_MatColor[index][iCamera][1][1] + 
							(1.0f - alpha)*m_MatColor[index][iCamera][0][1];
						error += fabs(ga - g);

						ba = alpha*m_MatColor[index][iCamera][1][2] + 
							(1.0f - alpha)*m_MatColor[index][iCamera][0][2];
						error += fabs(ba - b);

						if(error < min)
						{
							min = error;
							minAlpha = alpha;
						}
					}
				
					minAlpha = max(0.0f, min(1.0f, minAlpha));

					//minAlpha = min(m_MaxAlpha[row*m_Width + col][iCamera], minAlpha);

					m_Alpha[index][iCamera][0] = minAlpha;
					m_Alpha[index][iCamera][1] = minAlpha;
					m_Alpha[index][iCamera][2] = minAlpha;
				}
			}
		}
	}	

	for(iCamera=0;iCamera<m_NumCameras;iCamera++) {
		for(row=0;row<m_Height;row++) {
            int indrow = row*m_Width;

			for(col=0;col<m_Width;col++) {
                int index = indrow + col;

				pixSrc = m_SegmentImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) (255.0*m_Alpha[index][iCamera][0]));
				pixSrc->g = ((BYTE) (255.0*m_Alpha[index][iCamera][1]));
				pixSrc->b = ((BYTE) (255.0*m_Alpha[index][iCamera][2]));

				pixSrc = m_SmoothImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) m_MatColor[index][iCamera
				][0][0]);
				pixSrc->g = ((BYTE) m_MatColor[index][iCamera][0][1]);
				pixSrc->b = ((BYTE) m_MatColor[index][iCamera][0][2]);

				pixSrc = m_DisparityImage[iCamera].Ptr(col,row);

				pixSrc->r = ((BYTE) m_MatColor[index][iCamera][1][0]);
				pixSrc->g = ((BYTE) m_MatColor[index][iCamera][1][1]);
				pixSrc->b = ((BYTE) m_MatColor[index][iCamera][1][2]);
			}
		}
	}


} // combineAlphas




