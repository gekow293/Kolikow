#pragma once
#include "afxwin.h"
#include "GDIClass.h"
#include <string.h>
#include <afxstr.h>
#include <atlimage.h>
#include <atlstr.h>
using namespace std;
// Диалоговое окно CMFCApplication1Dlg
class FourierDlg : public CDialog
{
private:
public:
	FourierDlg(CWnd* pParent = nullptr);	// стандартный конструктор
	virtual ~FourierDlg(){};
	void fur(vector < complex < double>>& data, int is);
	void fur_2d(vector < vector < double >> Image);// vector < vector<double>>>& data
	void fur_2d_obrat();
	void gen_image();
	vector<double> AddNoise(vector<double>& Signal, double noise_procent, double E_s);
	void ClearSpectr();
	vector < vector < double >> LinOrLogScale(vector < vector < double >> SpectrImageCentr);
	vector<vector<double>>SpectrImageTransform(vector<vector<double>> SpectrImage);
	//vector<double> AddNoise(vector<double>& Signal, double noise_procent, vector<vector<double>>& Signal2d);
	vector <double> GaussRandom(int length);
	double Energy(vector<double>& S);
	double Energy2d(vector<vector<double>>& S);
	double Eps(vector<vector<double>> &Image1, vector<vector<double>> &Image2,int N);//адреса и третий аргумент добавил...а так не было!!
	void resizeMassives();
	vector<vector<double>> interpolate(vector<vector<double>> pixel, int oldWidth, int oldHeight, int newWidth, int newHeight);
	CWnd* ownerWndL,
		* ownerWndC,
		* ownerWndR,
		* frameWnd1,
		* frameWnd2,
		* frameWnd3,
		* frameWnd4;
	CDC* ownerDcL,
		* ownerDcC,
		* ownerDcR,
		* frameDc1,
		* frameDc2,
		* frameDc3,
		* frameDc4;
	CRect rL, rC, rR, r1, r2, r3, r4;
	bool flagPictureL, flagCleanSpectrPictureC, flagVosstPictureR;
	CDC pMemDcL, pMemDcC, pMemDcR, pMemDc1, pMemDc2, pMemDc3, pMemDc4;		// контекст рисования в памяти компьютера
	CImage* imgforcheck1, * imgforcheck2, * imgforcheck3, * imgforcheck4;
	CImage* img,* img1, * img2, * img3, * img4;
	HBITMAP hBmp1, hBmp2, hBmp3, hBmp4;
// Данные диалогового окна
	enum { IDD = IDD_MFCFOURIERAPP_DIALOG};
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV
	
protected:
	HICON m_hIcon;
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedGenImage();
	afx_msg void OnBtnClickedClearSpectr();
	afx_msg void OnBnClickedPushbuttonRestoreimage();
	afx_msg void OnBtnclickedLoadImage();
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedAboutDlg();
	afx_msg void OnBnClickedCheckLinOrLogScale();

	GDIClass v_PictureL;
	GDIClass v_PictureC;
	GDIClass v_PictureR;

	CStatic m_framepicture_1;
	CStatic m_framepicture_2;
	CStatic m_framepicture_3;
	CStatic m_framepicture_4;
	BOOL Check_box_img;
	BOOL Check_box_lin_or_lig_scale;
	CButton RadioButton1;
	CButton RadioButton2;
	CButton RadioButton3;
	CButton RadioButton4;
	CEdit textEdit1;
	CEdit textEdit2;
	int N;
	int xmax;
	int xmin;
	int ymax;
	int ymin;
	double A1, A2, A3, A4, A5;
	double X01, X02, X03, X04, X05;
	double Y01, Y02, Y03, Y04, Y05;
	double SigmaX1, SigmaX2, SigmaX3, SigmaX4, SigmaX5;
	double SigmaY1,SigmaY2, SigmaY3, SigmaY4, SigmaY5;
	vector <vector<complex<double>>> massPic;
	vector<double> t;
	vector < vector < double >> Image, ImageClear, ImageRestored, SpectrImageCentrScale,
	SpectrImageCentr, SpectrImageClearedCentr, SpectrImage, SpectrImageCleared;
	double SpectrEnergy00;
	double noise_energy;
	double noise_procent;
	double SpectrEnergyFull;
	double ClearProcent;
	DWORD dwThread;
	HANDLE hThread;
	DWORD dwThread1;
	HANDLE hThread1;
	DWORD dwThread2;
	HANDLE hThread2;
	DWORD dwThread3;
	HANDLE hThread3;
	double _error2;
	char tmpEps2_error[50];
	double _error1;
	char tmpEps1_error[50];
	double _error3;
	char tmpEps3_error[50];
};
