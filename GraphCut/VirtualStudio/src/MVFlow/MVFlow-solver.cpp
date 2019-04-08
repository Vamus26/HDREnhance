#include "MVFlow.h"
#include "ImageIO.h"

void MVFlow::computeGauB_SeidelIter()
{
	computePsiDiffImgs();

	CShape gridShape = this->currImg.Shape();

	uint nodeAddr = 0;
	for(int y = 0; y < gridShape.height; y++)
	for(int x = 0; x < gridShape.width;  x++, nodeAddr++)
	{
		Transform2x2<double> matA = computeMatA(x, y);

		Transform2x2<double> matA_Inv = matA.Inverse();

		Vector2<double> rhs = computeRHS(x, y);

		Vector2<double> lhs = matA_Inv * rhs;
		this->du[nodeAddr] = (float) lhs[0];
		this->dv[nodeAddr] = (float) lhs[1];
	}

	//bug
	//ImageProcessing::Operate(this->du, this->du, (float) gridShape.width,  ImageProcessing::OP_DIV);
	//ImageProcessing::Operate(this->dv, this->dv, (float) gridShape.height, ImageProcessing::OP_DIV);
}

Transform2x2<double> MVFlow::computeMatA(int x, int y)
{
	Transform2x2<double> matA;

	CShape gridShape = this->currImg.Shape();
	uint currNodeAddr = gridShape.NodeIndex(x, y);

	const Transform3x3<float> &currTensorS = this->tensorS[currNodeAddr];
	const Transform3x3<float> &currTensorT = this->tensorT[currNodeAddr];

	matA[0][0] = (this->params.alphaBC * this->psiDiff_D1[currNodeAddr] * currTensorS[0][0]) +
		         (this->params.alphaGC * this->psiDiff_D2[currNodeAddr] * currTensorT[0][0]);

	matA[1][1] = (this->params.alphaBC * this->psiDiff_D1[currNodeAddr] * currTensorS[1][1]) +
		         (this->params.alphaGC * this->psiDiff_D2[currNodeAddr] * currTensorT[1][1]);

	matA[0][1] = (this->params.alphaBC * this->psiDiff_D1[currNodeAddr] * currTensorS[0][1]) +
		         (this->params.alphaGC * this->psiDiff_D2[currNodeAddr] * currTensorT[0][1]);
	matA[1][0] = matA[0][1];

	double smoothnessTerm = 0.0;

	const int connectivity = 4;
	const int dx[connectivity]        = {-1,  1,  0,  0};
	const int dy[connectivity]        = { 0,  0,  1, -1};
	const int axisSize[connectivity]  = {gridShape.width, 
		                                 gridShape.width, 
										 gridShape.height, 
										 gridShape.height};

	int xCoordLimit = gridShape.width  - 1;
	int yCoordLimit = gridShape.height - 1;
	for(int iNeigh = 0; iNeigh < connectivity; iNeigh++)
	{
		int neighX = reflectCoord(x + dx[iNeigh], xCoordLimit);
		int neighY = reflectCoord(y + dy[iNeigh], yCoordLimit);
		uint neighNodeAddr = gridShape.NodeIndex(neighX, neighY);
		//bug
		int axisDenom = axisSize[iNeigh] * axisSize[iNeigh] * 2;
		//int axisDenom = 2;

		smoothnessTerm +=((this->psiDiff_S[currNodeAddr] + this->psiDiff_S[neighNodeAddr]) / axisDenom);
	}
	smoothnessTerm *= this->params.beta;

	matA[0][0] += smoothnessTerm;
	matA[1][1] += smoothnessTerm;

	return matA;
}


Vector2<double> MVFlow::computeRHS(int x, int y)
{
	Vector2<double> rhs;

	CShape gridShape = this->currImg.Shape();
	uint currNodeAddr = gridShape.NodeIndex(x, y);

	const Transform3x3<float> &currTensorS = this->tensorS[currNodeAddr];
	const Transform3x3<float> &currTensorT = this->tensorT[currNodeAddr];

	rhs[0] = (this->params.alphaBC * this->psiDiff_D1[currNodeAddr] * currTensorS[0][2] * -1.0f) -
		     (this->params.alphaGC * this->psiDiff_D2[currNodeAddr] * currTensorT[0][2]);

	rhs[1] = (this->params.alphaBC * this->psiDiff_D1[currNodeAddr] * currTensorS[1][2] * -1.0f) -
		     (this->params.alphaGC * this->psiDiff_D2[currNodeAddr] * currTensorT[1][2]);

	double smoothnessTermU = 0.0;
	double smoothnessTermV = 0.0;

	const int connectivity = 4;
	const int dx[connectivity]        = {-1,  1,  0,  0};
	const int dy[connectivity]        = { 0,  0,  1, -1};
	const int axisSize[connectivity]  = {gridShape.width, 
		                                 gridShape.width, 
										 gridShape.height, 
										 gridShape.height};

	int xCoordLimit = gridShape.width  - 1;
	int yCoordLimit = gridShape.height - 1;
	for(int iNeigh = 0; iNeigh < connectivity; iNeigh++)
	{
		int neighX = reflectCoord(x + dx[iNeigh], xCoordLimit);
		int neighY = reflectCoord(y + dy[iNeigh], yCoordLimit);
		uint neighNodeAddr = gridShape.NodeIndex(neighX, neighY);
		//bug
		int axisDenom = axisSize[iNeigh] * axisSize[iNeigh] * 2;
		//int axisDenom = 2;
		float smoothnessTerm = ((this->psiDiff_S[currNodeAddr] + this->psiDiff_S[neighNodeAddr]) / axisDenom);

		float uGrad = (this->u[neighNodeAddr] + this->du[neighNodeAddr]) - this->u[currNodeAddr];
		float vGrad = (this->v[neighNodeAddr] + this->dv[neighNodeAddr]) - this->v[currNodeAddr];

		smoothnessTermU += smoothnessTerm * uGrad;
		smoothnessTermV += smoothnessTerm * vGrad;
	}
	smoothnessTermU *= this->params.beta;
	smoothnessTermV *= this->params.beta;

	rhs[0] += smoothnessTermU;
	rhs[1] += smoothnessTermV;

	return rhs;
}

void MVFlow::computePsiDiffImgs()
{
	CShape gridShape = this->currImg.Shape();

	CFloatImage estU(gridShape);
	ImageProcessing::Operate(estU, this->u, this->du, ImageProcessing::OP_ADD);
	CFloatImage estU_dx(gridShape);
	CFloatImage estU_dy(gridShape);
	computeGradImgs(estU, estU_dx, estU_dy);

	CFloatImage estV(gridShape);	
	ImageProcessing::Operate(estV, this->v, this->dv, ImageProcessing::OP_ADD);
	CFloatImage estV_dx(gridShape);
	CFloatImage estV_dy(gridShape);
	computeGradImgs(estV, estV_dx, estV_dy);

	//ImageProcessing::Operate(estU_dx, estU_dx, (float) gridShape.width,  ImageProcessing::OP_MUL);
	//ImageProcessing::Operate(estU_dy, estU_dy, (float) gridShape.height, ImageProcessing::OP_MUL);
	//ImageProcessing::Operate(estV_dx, estV_dx, (float) gridShape.width,  ImageProcessing::OP_MUL);
	//ImageProcessing::Operate(estV_dy, estV_dy, (float) gridShape.height, ImageProcessing::OP_MUL);


	uint nodeAddr = 0;
	for(int y = 0; y < gridShape.height; y++)
	for(int x = 0; x < gridShape.width;  x++, nodeAddr++)
	{
		Vector3<float> du_dv_1(this->du[nodeAddr], 
							   this->dv[nodeAddr],
							   1.0f);
		float psiDiffArg;

		psiDiffArg = (this->tensorS[nodeAddr] * du_dv_1).dot(du_dv_1);
		this->psiDiff_D1[nodeAddr] = psiDiffD(psiDiffArg);

		psiDiffArg = (this->tensorT[nodeAddr] * du_dv_1).dot(du_dv_1);
		this->psiDiff_D2[nodeAddr] = psiDiffD(psiDiffArg);

		psiDiffArg  = (estU_dx[nodeAddr] * estU_dx[nodeAddr]);
		psiDiffArg += (estU_dy[nodeAddr] * estU_dy[nodeAddr]);
		psiDiffArg += (estV_dx[nodeAddr] * estV_dx[nodeAddr]);
		psiDiffArg += (estV_dy[nodeAddr] * estV_dy[nodeAddr]);
		this->psiDiff_S[nodeAddr] = psiDiffS(psiDiffArg);
	}
}

