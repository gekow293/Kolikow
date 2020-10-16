#include "pch.h"
#include "framework.h"
#include "FourierApp.h"
#include "FourierDlg.h"
#include "GDIClass.h"
#include "afxdialogex.h"
#include <iostream>
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();
	// Данные диалогового окна
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV
// Реализация
protected:
	DECLARE_MESSAGE_MAP()
public:
};
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


//Диалоговое окно FourierDlg
FourierDlg::FourierDlg(CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_MFCFOURIERAPP_DIALOG, pParent)
	, N(256)
	, A1(1.5), X01(60), Y01(60), SigmaX1(20), SigmaY1(20)
	, A2(1.5), X02(190), Y02(60), SigmaX2(20), SigmaY2(20)
	, A3(1), X03(125), Y03(125), SigmaX3(20), SigmaY3(20)
	, A4(1.5), X04(60), Y04(190), SigmaX4(20), SigmaY4(20)
	, A5(1.5), X05(190), Y05(190), SigmaX5(20), SigmaY5(20),
	ownerWndL(NULL), ownerWndR(NULL),ownerWndC(NULL), 
	frameWnd1(NULL), frameWnd2(NULL), frameWnd3(NULL), frameWnd4(NULL),
	ownerDcL(NULL), ownerDcC(NULL), ownerDcR(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void FourierDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SIZEN, N);
	DDX_Text(pDX, IDC_EDIT_Amplituda1, A1);
	DDX_Text(pDX, IDC_EDIT_Amplituda2, A2);
	DDX_Text(pDX, IDC_EDIT_Amplituda3, A3);
	DDX_Text(pDX, IDC_EDIT_Amplituda4, A4);
	DDX_Text(pDX, IDC_EDIT_Amplituda5, A5);
	DDX_Text(pDX, IDC_EDITX1, X01);
	DDX_Text(pDX, IDC_EDITX2, X02);
	DDX_Text(pDX, IDC_EDITX3, X03);
	DDX_Text(pDX, IDC_EDITX4, X04);
	DDX_Text(pDX, IDC_EDITX5, X05);
	DDX_Text(pDX, IDC_EDITY1, Y01);
	DDX_Text(pDX, IDC_EDITY2, Y02);
	DDX_Text(pDX, IDC_EDITY3, Y03);
	DDX_Text(pDX, IDC_EDITY4, Y04);
	DDX_Text(pDX, IDC_EDITY5, Y05);
	DDX_Text(pDX, IDC_EDITSigX1, SigmaX1);
	DDX_Text(pDX, IDC_EDITSigX2, SigmaX2);
	DDX_Text(pDX, IDC_EDITSigX3, SigmaX3);
	DDX_Text(pDX, IDC_EDITSigX4, SigmaX4);
	DDX_Text(pDX, IDC_EDITSigX5, SigmaX5);
	DDX_Text(pDX, IDC_EDITSigY1, SigmaY1);
	DDX_Text(pDX, IDC_EDITSigY2, SigmaY2);
	DDX_Text(pDX, IDC_EDITSigY3, SigmaY3);
	DDX_Text(pDX, IDC_EDITSigY4, SigmaY4);
	DDX_Text(pDX, IDC_EDITSigY5, SigmaY5);
	DDX_Text(pDX, IDC_EDIT_NOIZE, noise_procent);
	DDX_Check(pDX, IDC_CHECK_BOX_IMG1, Check_box_img);
	DDX_Control(pDX, IDC_EDITEps, textEdit1);
	DDX_Control(pDX, IDC_EDITEps1, textEdit2);
	DDX_Control(pDX, IDC_RADIO1, RadioButton1);
	DDX_Control(pDX, IDC_RADIO2, RadioButton2);
	DDX_Control(pDX, IDC_RADIO3, RadioButton3);
	DDX_Control(pDX, IDC_RADIO4, RadioButton4);
	DDX_Text(pDX, IDC_EDIT_LIMIT, ClearProcent);
	DDX_Control(pDX, IDP_PICTURE_1, m_framepicture_1);
	DDX_Control(pDX, IDP_PICTURE_2, m_framepicture_2);
	DDX_Control(pDX, IDP_PICTURE_3, m_framepicture_3);
	DDX_Control(pDX, IDP_PICTURE_4, m_framepicture_4);
	DDX_Control(pDX, IDP_PICTURE_L, v_PictureL);
	DDX_Control(pDX, IDP_PICTURE_C, v_PictureC);
	DDX_Control(pDX, IDP_PICTURE_R, v_PictureR);
	DDX_Check(pDX, IDC_CHECK_LIN_OR_LOG_SCALE, Check_box_lin_or_lig_scale);
}

BEGIN_MESSAGE_MAP(FourierDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_SYSCOMMAND()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDB_GEN_IMAGE, &FourierDlg::OnBnClickedGenImage)
	ON_BN_CLICKED(IDB_PushButtonClearSpectr, &FourierDlg::OnBtnClickedClearSpectr)
	ON_BN_CLICKED(IDB_PushButton_RestoreImage, &FourierDlg::OnBnClickedPushbuttonRestoreimage)
	ON_BN_CLICKED(IDB_BtnLoadImage, &FourierDlg::OnBtnclickedLoadImage)
	ON_BN_CLICKED(IDOK, &FourierDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &FourierDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_ABOUT_DLG, &FourierDlg::OnBnClickedAboutDlg)
	ON_BN_CLICKED(IDC_RADIO1, &FourierDlg::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &FourierDlg::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &FourierDlg::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_RADIO4, &FourierDlg::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_CHECK_LIN_OR_LOG_SCALE, &FourierDlg::OnBnClickedCheckLinOrLogScale)
END_MESSAGE_MAP()

// Обработчики сообщений FourierDlg// 

BOOL FourierDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}
	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);

	img1 = new CImage;
	img1->Load(_T("pic55.png"));
	hBmp1 = img1->Detach();
	m_framepicture_1.SetBitmap(hBmp1);
	m_framepicture_1.SetWindowPos(NULL, 0, 0, 135, 145, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	img2 = new CImage;
	img2->Load(_T("pic77.png"));
	hBmp2 = img2->Detach();
	m_framepicture_2.SetBitmap(hBmp2);
	m_framepicture_2.SetWindowPos(NULL, 0, 0, 135, 145, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	img3 = new CImage;
	img3->Load(_T("pic88.png"));
	hBmp3 = img3->Detach();
	m_framepicture_3.SetBitmap(hBmp3);
	m_framepicture_3.SetWindowPos(NULL, 0, 0, 135, 145, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
	img4 = new CImage;
	img4->Load(_T("pic8080.png"));
	hBmp4 = img4->Detach();
	m_framepicture_4.SetBitmap(hBmp4);
	m_framepicture_4.SetWindowPos(NULL, 0, 0, 135, 145, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);

	Check_box_lin_or_lig_scale = false;
	ownerWndL = GetDlgItem(IDP_PICTURE_L);
	ownerDcL = ownerWndL->GetDC();
	ownerWndL->GetClientRect(&rL);
	flagPictureL = false;
	ownerWndC = GetDlgItem(IDP_PICTURE_C);
	ownerDcC = ownerWndC->GetDC();
	ownerWndC->GetClientRect(&rC);
	flagCleanSpectrPictureC = false;
	ownerWndR = GetDlgItem(IDP_PICTURE_R);
	ownerDcR = ownerWndR->GetDC();
	ownerWndR->GetClientRect(&rR);
	flagVosstPictureR = false;
	Check_box_img = false;
	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void FourierDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

HCURSOR FourierDlg::OnQueryDragIcon()       // Система вызывает эту функцию для получения отображения курсора при перемещении
{											//  свернутого окна.
	return static_cast<HCURSOR>(m_hIcon);
}

void FourierDlg::OnPaint()
{
	//UpdateData(TRUE);
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;
		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
		//if (massPic.size() != 0)
		//	{
		//	xmax = massPic.size();
		//	ymax = massPic.size();
		//	if (flagVosstPictureR==true)//OnBnClickedPushbuttonRestoreimage();
		//	{
		//		v_PictureR.GradientCell(ImageRestored);
		//	}
			//if (flagPictureL==true)
			//{
			//	if (Check_box_img && flagCleanSpectrPictureC == true)//OnBtnclickedLoadImage
			//	{
			//		v_PictureL.GradientCell(Image);
			//		v_PictureC.GradientCell(SpectrImageCentr);
			//	}
			//}
		//	if (flagCleanSpectrPictureC==true)// OnBtnClickedClearSpectr()
		//	{
		//		v_PictureC.GradientCell(SpectrImageClearedCentr);
		//	}
		//	if (flagPictureL && flagCleanSpectrPictureC==true)//OnBnClickedGenImage
		//	{
		//		v_PictureL.GradientCell(Image);
		//		v_PictureC.GradientCell(SpectrImageCentr);
		//	}
		//}
	}
}

////--------поток0--------->
DWORD WINAPI MyProc(PVOID pv)//GenImage
{
	FourierDlg* p = (FourierDlg*)pv;
	p->gen_image();
	p->fur_2d(p->Image);
	//p->LinOrLogScale(p->SpectrImageCentr);
	p->v_PictureL.GradientCell(p->Image);
	p->v_PictureC.GradientCell(p->LinOrLogScale(p->SpectrImageCentr));
	p->N,
	p->X01, p->X02, p->X03, p->X04, p->X05;
	p->Y01, p->Y02, p->Y03, p->Y04, p->Y05;
	p->SigmaX1, p->SigmaX2, p->SigmaX3, p->SigmaX4, p->SigmaX5;
	p->SigmaY1, p->SigmaY2, p->SigmaY3, p->SigmaY4, p->SigmaY5;
	p->A1, p->A2, p->A3, p->A4, p->A5;
	p->Invalidate();
	p->_error3 = p->Eps(p->ImageClear, p->Image, p->N);
	sprintf_s(p->tmpEps3_error, "%5.10f", p->_error3);
	p->textEdit1.SetWindowTextW((CString)p->tmpEps3_error);
	return 0;
}

////--------поток1--------->
DWORD WINAPI MyProc1(PVOID pv)//ClearSpectr
{
	FourierDlg* p = (FourierDlg*)pv;
	p->ClearSpectr();
	p->v_PictureC.GradientCell(p->SpectrImageClearedCentr);

	p->Invalidate();
	return 0;
}

////--------поток2--------->
DWORD WINAPI MyProc2(PVOID pv)//RestoreImage
{
	FourierDlg* p = (FourierDlg*)pv;
	p->fur_2d_obrat();
	p->v_PictureR.GradientCell(p->ImageRestored);
	p->Invalidate();
	p->_error2 = p->Eps(p->ImageClear, p->ImageRestored, p->N);
	sprintf_s(p->tmpEps2_error, "%5.10f", p->_error2);
	p->textEdit2.SetWindowTextW((CString)p->tmpEps2_error);
	return 0;
}

////--------поток3-------->
DWORD WINAPI MyProc3(PVOID pv)//LoadImage
{
	FourierDlg* p = (FourierDlg*)pv;
	p->fur_2d(p->Image);
	//p->LinOrLogScale(p->SpectrImageCentr);
	p->v_PictureL.GradientCell(p->Image);
	p->v_PictureC.GradientCell(p->LinOrLogScale(p->SpectrImageCentr));
	p->N;
	p->Invalidate();
	p->_error3 = p->Eps(p->ImageClear, p->Image, p->N);
	sprintf_s(p->tmpEps3_error, "%5.10f", p->_error3);
	p->textEdit1.SetWindowTextW((CString)p->tmpEps3_error);
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////рабочие кнопки
void FourierDlg::OnBnClickedGenImage()
{
	UpdateData(TRUE);
	resizeMassives();
	flagPictureL = true;
	flagCleanSpectrPictureC = true;
	////-------------------->
	hThread2 = CreateThread(NULL, 0, MyProc, this, 0, &dwThread2);
	////-------------------->
	UpdateData(FALSE);
}

void FourierDlg::OnBtnClickedClearSpectr()
{
	UpdateData(TRUE);
	////-------------------->
	flagCleanSpectrPictureC = true;
	hThread1 = CreateThread(NULL, 0, MyProc1, this, 0, &dwThread1);
	////-------------------->
	UpdateData(FALSE);
}

void FourierDlg::OnBnClickedPushbuttonRestoreimage()
{
	UpdateData(true);
	////-------------------->
	flagVosstPictureR = true;
	hThread = CreateThread(NULL, 0, MyProc2, this, 0, &dwThread);
	////-------------------->
}

void FourierDlg::OnBtnclickedLoadImage()
{
	UpdateData(TRUE);
	resizeMassives();
	flagPictureL = true;
	Check_box_img = true;
	flagCleanSpectrPictureC = true;
	if (Check_box_img)
	{
		img = new CImage;
		if (RadioButton1.GetCheck())
		{	
			img->Load(_T("pic5.png"));
		}
		if (RadioButton2.GetCheck())
		{
			img->Load(_T("pic7.png"));
		}
		if (RadioButton3.GetCheck())
		{
			img->Load(_T("pic8.png"));
		}
		if (RadioButton4.GetCheck())
		{
			img->Load(_T("pic80.png"));
		}
		Image.resize(img->GetWidth());
		for (int i = 0; i < img->GetHeight(); i++)
		{
			Image[i].resize(img->GetWidth());
		}
		for (int i = 0; i < N; i++)
		{
			for (int j = 0; j < N; j++)
			{
				Image[i][j] = GetRValue(img->GetPixel(i, j)) * 0.299f + GetGValue(img->GetPixel(i, j)) * 0.587f + GetBValue(img->GetPixel(i, j)) * 0.114f;
			}
		}
	}
	else
	{
		cout << " Press - <<Change to image>> and select the image ";
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			ImageClear[i][j] = Image[i][j];
		}
	}
	double E_s = Energy2d(Image);
	for (int i = 0; i < N; i++)
	{
		Image[i] = AddNoise(Image[i], noise_procent, E_s);
	}
	if (NULL == img)
		delete img;
	else
	{
		img = NULL;
		delete img;
	}
	//---------------->
	hThread3 = CreateThread(NULL, 0, MyProc3, this, 0, &dwThread3);
	//---------------->
	UpdateData(FALSE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//...............................................................................//ФУРЬЕ
 void FourierDlg::fur(vector < complex < double>>& data, int is)
{
	int i, j, istep, n;
	int m, mmax;
	n = data.size();
	double r, r1, theta, w_r, w_i, temp_r, temp_i;
	double pi = 3.1415926f;

	r = pi * is;
	j = 0;
	for (i = 0; i < n; i++)
	{
		if (i < j)
		{
			temp_r = data[j].real();
			temp_i = data[j].imag();
			data[j] = data[i];
			data[i] = temp_r + complex <double>(0, 1) * temp_i;
		}
		m = n >> 1;
		while (j >= m) { j -= m; m = (m + 1) / 2; }
		j += m;
	}
	mmax = 1;
	while (mmax < n)
	{
		istep = mmax << 1;
		r1 = r / (double)mmax;
		for (m = 0; m < mmax; m++)
		{
			theta = r1 * m;
			w_r = (double)cos((double)theta);
			w_i = (double)sin((double)theta);
			for (i = m; i < n; i += istep)
			{
				j = i + mmax;
				temp_r = w_r * data[j].real() - w_i * data[j].imag();
				temp_i = w_r * data[j].imag() + w_i * data[j].real();
				data[j] = (data[i].real() - temp_r) + complex <double>(0, 1) * (data[i].imag() - temp_i);
				data[i] += (temp_r)+complex <double>(0, 1) * (temp_i);
			}
		}
		mmax = istep;
	}
	if (is > 0)
		for (i = 0; i < n; i++)
		{
			data[i] /= (double)n;
		}
}

void FourierDlg::fur_2d(vector < vector < double >> Image)
{
	vector < complex < double >> data_in_line1;
	massPic.resize(N);
	for (int i = 0; i < massPic.size(); i++)
		massPic[i].resize(N);
	data_in_line1.resize(N);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			data_in_line1[j] = Image[i][j];
		}
		fur(data_in_line1,-1);
		for (int j = 0; j < N; j++)
		{
			massPic[i][j] = data_in_line1[j];
		}
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			data_in_line1[j] = massPic[j][i];
		}
		fur(data_in_line1,-1);
		for (int j = 0; j < N; j++)
		{
			massPic[j][i] = data_in_line1[j];
			SpectrImage[j][i] = sqrt(massPic[j][i].real() * massPic[j][i].real() + massPic[j][i].imag() * massPic[j][i].imag());
		}
	}

	SpectrImage[0][0] = 0;
	SpectrImageCentr = SpectrImageTransform(SpectrImage);

	data_in_line1.clear();
}

void FourierDlg::fur_2d_obrat()
{
	vector < complex < double >> data_in_line1;
	data_in_line1.resize(N);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			data_in_line1[j] = massPic[i][j];//ij
		}
		fur(data_in_line1,1);
		for (int j = 0; j < N; j++)
		{
			massPic[i][j] = data_in_line1[j];
		}
	}

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			data_in_line1[j] = massPic[j][i];//ji
		}
		fur(data_in_line1,1);
		for (int j = 0; j < N; j++)
		{
			ImageRestored[j][i] = sqrt(data_in_line1[j].real() * data_in_line1[j].real() + data_in_line1[j].imag() * data_in_line1[j].imag());
		}
	}
	data_in_line1.clear();
}
//...............................................................................//


//////////////........................<< функции вычислений >>..........................///////////////////
void FourierDlg::gen_image()
{
	for (int i = 0; i < t.size(); i++)
	{
		t[i] = i;
	}	
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			Image[i][j] = A1 * exp(-((((double(i) - X01) * (double(i) - X01)) / (SigmaX1 * SigmaX1)) + (((double(j) - Y01) * (double(j) - Y01)) / (SigmaY1 * SigmaY1))))
				+ A2 * exp(-((((double(i) - X02) * (double(i) - X02)) / (SigmaX2 * SigmaX2)) + (((double(j) - Y02) * (double(j) - Y02)) / (SigmaY2 * SigmaY2))))
				+ A3 * exp(-(((double(i) - X03) * (double(i) - X03) / (SigmaX3 * SigmaX3)) + ((double(j) - Y03) * (double(j) - Y03) / (SigmaY3 * SigmaY3))))
				+ A4 * exp(-(((double(i) - X04) * (double(i) - X04) / (SigmaX4 * SigmaX4)) + ((double(j) - Y04) * (double(j) - Y04) / (SigmaY4 * SigmaY4))))
				+ A5 * exp(-(((double(i) - X05) * (double(i) - X05) / (SigmaX5 * SigmaX5)) + ((double(j) - Y05) * (double(j) - Y05) / (SigmaY5 * SigmaY5))));
		}
	}
	ImageClear.resize(N);
	for (int i = 0; i < ImageClear.size(); i++)
	{
		ImageClear[i].resize(N);
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			ImageClear[i][j] = Image[i][j];
		}
	}
	double E_s = Energy2d(Image);
	for (int i = 0; i < N; i++)
	{
		Image[i] = AddNoise(Image[i], noise_procent, E_s);
	}
}

vector<double> FourierDlg::AddNoise(vector<double>& Signal, double noise_procent, double E_s)//noise_procent
{
	vector<double> noise = GaussRandom(Signal.size());
	double E_n = Energy(noise);
	double norm_koeff = sqrt(E_s * (noise_procent / 100.) / E_n);
	std::vector<double> N;
	for (int i = 0; i < Signal.size(); i++)
	{
		double real = Signal[i] + norm_koeff * noise[i];
		N.push_back(real);
	}
	noise_energy = E_n * norm_koeff * norm_koeff;
	return N;
}

vector<vector<double>> FourierDlg::SpectrImageTransform(vector<vector<double>> SpectrImage)
{
	std::vector<vector<double>> SpectrImageTmp;
	SpectrImageTmp.resize(N / 2);
	for (int i = 0; i < SpectrImageTmp.size(); i++)
	{
		SpectrImageTmp[i].resize(N / 2);
	}
	for (int i = 0; i < N / 2; i++)
	{
		for (int j = 0; j < N / 2; j++)
		{
			SpectrImageTmp[i][j] = SpectrImage[i + N / 2][j + N / 2];
		}
	}
	for (int i = 0; i < N / 2; i++)
	{
		for (int j = 0; j < N / 2; j++)
		{
			SpectrImage[i + N / 2][j + N / 2] = SpectrImage[i][j];
		}
	}
	for (int i = 0; i < N / 2; i++)
	{
		for (int j = 0; j < N / 2; j++)
		{
			SpectrImage[i][j] = SpectrImageTmp[i][j];
		}
	}
	for (int i = 0; i < N / 2; i++)
	{
		for (int j = 0; j < N / 2; j++)
		{
			SpectrImageTmp[i][j] = SpectrImage[i + N / 2][j];
		}
	}
	for (int i = 0; i < N / 2; i++)
	{
		for (int j = 0; j < N / 2; j++)
		{
			SpectrImage[i + N / 2][j] = SpectrImage[i][j + N / 2];
		}
	}
	for (int i = 0; i < N / 2; i++)
	{
		for (int j = 0; j < N / 2; j++)
		{
			SpectrImage[i][j + N / 2] = SpectrImageTmp[i][j];
		}
	}
	return SpectrImage;
}

void FourierDlg::ClearSpectr()
{
	Image[0][0] = SpectrEnergy00;
	SpectrImage[0][0] = 0;
	SpectrImage = LinOrLogScale(SpectrImage);
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			SpectrImageCleared[i][j] = SpectrImage[i][j];
		}
	}
	SpectrEnergy00 = SpectrImage[0][0] * SpectrImage[0][0];
	SpectrEnergyFull = 0;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			SpectrEnergyFull += SpectrImage[i][j] * SpectrImage[i][j];
		}
	}
	int range = 0;
	double SpectrEnergy;
	do
	{
		SpectrEnergy = 0;
		for (int i = 0; i < range; i++)
		{
			for (int j = 0; j < range; j++)
			{
				SpectrEnergy += SpectrImage[i][j] * SpectrImage[i][j];
			}
		}
		for (int i = N - 1; i > N - 1 - range; i--)
		{
			for (int j = N - 1; j > N - 1 - range; j--)
			{
				SpectrEnergy += SpectrImage[i][j] * SpectrImage[i][j];
			}
		}
		for (int i = N - 1; i > N - 1 - range; i--)
		{
			for (int j = 0; j < range; j++)
			{
				SpectrEnergy += SpectrImage[i][j] * SpectrImage[i][j];
			}
		}
		for (int i = 0; i < range; i++)
		{
			for (int j = N - 1; j > N - 1 - range; j--)
			{
				SpectrEnergy += SpectrImage[i][j] * SpectrImage[i][j];
			}
		}

		range++;
	} while (SpectrEnergy < (ClearProcent* SpectrEnergyFull)/100);
	
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (i > range&& i < (N - range) || (j > range&& j < (N - range)))
			{
				SpectrImageCleared[i][j] = 0;
				massPic[i][j]=0;
			}
		}
	}
	
	SpectrImageClearedCentr = SpectrImageTransform(SpectrImageCleared);

	double Max = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (SpectrImageClearedCentr[i][j] > Max) {
				Max = SpectrImageClearedCentr[i][j];
			}
		}
	}
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			if (i == (N / 2 + range) && j > N / 2 - range - 1 && j < N / 2 + range + 1)
			{
				SpectrImageClearedCentr[i + 1][j] = Max;
			}
			if (j == (N / 2 + range - 1) && i > N / 2 - range - 1 && i < N / 2 + range + 1)
			{
				SpectrImageClearedCentr[i][j + 1] = Max;
			}
			if (i == (N / 2 - range) && j > N / 2 - range - 1 && j < N / 2 + range + 1)
			{
				SpectrImageClearedCentr[i - 1][j] = Max;
			}
			if (j == (N / 2 - range - 1) && i > N / 2 - range - 1 && i < N / 2 + range + 1)
			{
				SpectrImageClearedCentr[i][j + 1] = Max;
			}
		}
	}
}

vector < vector < double >> FourierDlg::LinOrLogScale(vector < vector < double >> SpectrImageCentr) {
	double Max = 0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (SpectrImageCentr[i][j] > Max) {
				Max = SpectrImageCentr[i][j];
			}
		}
	}
	if (Check_box_lin_or_lig_scale == false) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				SpectrImageCentrScale[i][j] = ((log(1 + SpectrImageCentr[i][j])) / (log(1 + Max))) * 255;
			}
		}
	}
	if (Check_box_lin_or_lig_scale == true) {
		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				SpectrImageCentrScale[i][j] = (SpectrImageCentr[i][j] / Max) * 255;
			}
		}
	}
	return SpectrImageCentrScale;
}

vector<double> FourierDlg::GaussRandom(int length)
{
	std::vector <double> Res;

	for (int i = 0; i < length; i++)
	{
		double rand_i = 0;

		for (int k = 0; k < 12; k++)
		{
			rand_i += ((2 * (double)rand() / RAND_MAX) - 1);
		}

		double rand_norm_i = rand_i / 12;
		Res.push_back(rand_norm_i);
	}

	return  Res;
}

double FourierDlg::Energy(vector<double>& S)
{
	double summ = 0;
	for (int i = 0; i < S.size(); i++)
	{
		summ += S[i] * S[i];
	}
	return summ;
}

double FourierDlg::Energy2d(vector<vector<double>>& S)
{
	double summ = 0;
	for (int i = 0; i < S.size(); i++)
	{
		for (int j = 0; j < S.size(); j++)
		{
			summ += S[i][j] * S[i][j];
		}

	}
	return summ;
}

double FourierDlg::Eps(vector<vector<double>> &Image1, vector<vector<double>> &Image2, int N)//временно добавил указатели к переменным и третий аргумент N
{
	double res = 0;
	double sum1 = 0;
	double sum2 = 0;
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			sum1 += double((Image1[i][j] - Image2[i][j]) * (Image1[i][j] - Image2[i][j]));
			sum2 += double(Image1[i][j] * Image1[i][j]);
		}
	}
	res = sum1 / sum2;
	return res;
}

void FourierDlg::resizeMassives()
{
	//t.resize(N);
	Image.resize(N);
	for (int i = 0; i < Image.size(); i++)
	{
		Image[i].resize(N);
	}
	ImageClear.resize(N);
	for (int i = 0; i < ImageClear.size(); i++)
	{
		ImageClear[i].resize(N);
	}
	ImageRestored.resize(N);
	for (int i = 0; i < ImageRestored.size(); i++)
	{
		ImageRestored[i].resize(N);
	}
	SpectrImage.resize(N);
	for (int i = 0; i < SpectrImage.size(); i++)
	{
		SpectrImage[i].resize(N);
	}
	SpectrImageCentr.resize(N);
	for (int i = 0; i < SpectrImageCentr.size(); i++)
	{
		SpectrImageCentr[i].resize(N);
	}
	SpectrImageCleared.resize(N);
	for (int i = 0; i < SpectrImageCleared.size(); i++)
	{
		SpectrImageCleared[i].resize(N);
	}
	SpectrImageClearedCentr.resize(N);
	for (int i = 0; i < SpectrImageClearedCentr.size(); i++)
	{
		SpectrImageClearedCentr[i].resize(N);
	}
	SpectrImageCentrScale.resize(N);
	for (int i = 0; i < SpectrImageCentrScale.size(); i++)
	{
		SpectrImageCentrScale[i].resize(N);
	}
}

void FourierDlg::OnDestroy()
{
	CDialog::OnDestroy();
}
///////////////////...................................................................................

void FourierDlg::OnBnClickedRadio1()
{
		RadioButton1.SetCheck(1);
}

void FourierDlg::OnBnClickedRadio2()
{
	RadioButton2.SetCheck(1);
}

void FourierDlg::OnBnClickedRadio3()
{
	RadioButton3.SetCheck(1);
}

void FourierDlg::OnBnClickedRadio4()
{
	RadioButton4.SetCheck(1);
}

void FourierDlg::OnBnClickedCancel()
{
	CDialog::OnCancel();
}

void FourierDlg::OnBnClickedOk()
{
	CDialog::OnOK();
}

void FourierDlg::OnBnClickedCheckLinOrLogScale()
{
	if (Check_box_lin_or_lig_scale = false) {
		Check_box_lin_or_lig_scale = true;
	}
	else {
		Check_box_lin_or_lig_scale = false;
	}
}


void FourierDlg::OnBnClickedAboutDlg()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}

vector<vector<double>> FourierDlg::interpolate(vector<vector<double>> pixel, int oldWidth, int oldHeight, int newWidth, int newHeight) {
	vector<vector<double>> temp(newWidth*newHeight);
	double xRatio = oldWidth / (double)newWidth;
	double yRatio = oldHeight / (double)newHeight;
	double px, py;
	for (int i = 0; i < newHeight; i++) {
		for (int j = 0; j < newWidth; j++) {
			px = floor(j*xRatio);
			py = floor(i*yRatio);
			temp[(i*newWidth)][j*newHeight] = pixel[(int)((py*oldWidth))][(int)((py*newHeight))];
		}
	}
	return temp;
}
