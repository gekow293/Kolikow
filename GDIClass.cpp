#include "pch.h"
#include <gdiplus.h>
#include "GDIClass.h"
#include "FourierApp.h"
#include "FourierDlg.h"
using namespace Gdiplus;
IMPLEMENT_DYNAMIC(GDIClass, CStatic)
ULONG_PTR gdiPlusToken;

GDIClass::GDIClass()
{
	massPic.resize(N);
	for (auto& it : massPic)
	{
		it.resize(N);
	}
	GdiplusStartupInput gdiPlusStartapInput;
	if (GdiplusStartup(&gdiPlusToken, &gdiPlusStartapInput, NULL) != Ok)
	{
		MessageBox(L"Init error", L"Error Message", MB_OK);
		exit(0);
	}
}

GDIClass::~GDIClass()
{
	GdiplusShutdown(gdiPlusToken);
}

BEGIN_MESSAGE_MAP(GDIClass, CStatic)
END_MESSAGE_MAP()

 void GDIClass::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// std::vector <vector<GDIDot>> massPic(N, std::vector<GDIDot>(N));
	 Graphics g(lpDrawItemStruct->hDC);
	 Bitmap Map(lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left + 1,
		 lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top + 1,
		 &g);
	 Graphics g2(&Map);
	 g2.SetSmoothingMode(SmoothingModeHighSpeed);
	 Color A((BYTE)0, (BYTE)0, (BYTE)0);
	 g2.Clear(A);
	 Matrix matr;
	 g2.ResetTransform();

	 Pen p(Color::Blue, 2);
	 Pen pred(Color::Red, 1);
	 Pen psetka(Color::LightGray, 0.001);
	 SolidBrush kist(Color::White);
	 Gdiplus::REAL kx = (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left) / (double)(xmax - xmin); //масштабные коэффициенты
	 Gdiplus::REAL ky = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top) / (double)(ymax - ymin);
	 matr.Scale(kx, ky);
	 g2.SetTransform(&matr);
	 if (massPic.size() != 0)
	 {
		 for (int i = 0; i < massPic.size(); i++)
		 {
			 for (int j = 0; j < massPic[i].size(); j++)
			 {
				 SolidBrush br(Color(massPic[i][j].R, massPic[i][j].G, massPic[i][j].B));
				 g2.FillRectangle(&br, i, j, 1., 1.);
			 }
		 }
	 }
	 g.DrawImage(&Map, 0, 0);
}

 void GDIClass::GradientCell(vector<vector<double>> Data)
 {
	 massPic.resize(N);
	 for (int i = 0; i < massPic.size(); i++)
		 massPic[i].resize(Data[i].size());

	 if (massPic.size() != 0)
	 {
		 double GradMin = 0;
		 double GradMax = 0;
		 MINMAX(Data, GradMin, GradMax);
		 double GradRange = GradMax - GradMin;
		 for (int i = 0; i < Data.size(); i++)
		 {
			 for (int j = 0; j < Data[i].size(); j++)
			 {
				 double buf = (double)((Data[i][j] - GradMin) * 255 / GradRange);
				 massPic[i][j].R = (BYTE)buf;
				 massPic[i][j].G = (BYTE)buf;
				 massPic[i][j].B = (BYTE)buf;
			 }
		 }
	 }
 }

 void GDIClass::MINMAX(vector<vector<double>> Data, double& Min, double& Max)
 {
	 if (Data.size() == 0)return;
	 Min = Max = 0;
	 for (int i = 0; i < Data.size(); i++)
	 {
		 for (int j = 0; j < Data[i].size(); j++)
		 {
			 if (Data[i][j] < Min)Min = Data[i][j];
			 if (Data[i][j] > Max)Max = Data[i][j];
		 }
	 }
 }
