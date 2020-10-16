#pragma once
//#include "pch.h"
#include <afxwin.h>
#include <math.h>
#include <vector>
#include <complex>

using namespace std;
#define M_PI 3.141592653
struct GDIDot 
{
	BYTE R;
	BYTE G;
	BYTE B;
};
class GDIClass :
	public CStatic
{
	DECLARE_DYNAMIC(GDIClass)
public:
	GDIClass();
	virtual ~GDIClass();
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
private:
	int xmax = 256;
	int xmin = 0;
	int ymax = 256;
	int ymin = 0;
protected:
	DECLARE_MESSAGE_MAP()
public:
	void MINMAX(vector<vector<double>> Data, double& Min, double& Max);
	void GradientCell(vector<vector<double>> Data);
	vector <vector<GDIDot>> massPic;
	int N = 256;
	
};