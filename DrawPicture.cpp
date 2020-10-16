// DrawPicture.cpp: файл реализации
//

#include "stdafx.h"
#include "AnimSignal.h"
#include "DrawPicture.h"
#include <gdiplus.h>
#include <math.h>

using namespace Gdiplus;

ULONG_PTR token;

double frec;
double razn; // Глобальные переменный
double fi = 0;
// DrawPicture

IMPLEMENT_DYNAMIC(DrawPicture, CStatic)


DrawPicture::DrawPicture()
{
	GdiplusStartupInput input;
	Status s;
	s = GdiplusStartup(&token, &input, NULL);
	if (s != Ok) MessageBox(L"Error #404: Not Found!", L"Try again later", MB_OK | MB_ICONERROR);

	frec = 100; // первая частота (w1)
	razn = 10; // разность частот (w2 - w1)

}

DrawPicture::~DrawPicture()
{
}

float func(float t, double fi)
{
	return (cos((frec)*t + fi) + cos((razn + (frec))*t));
}



BEGIN_MESSAGE_MAP(DrawPicture, CStatic)
END_MESSAGE_MAP()



// обработчики сообщений DrawPicture




void DrawPicture::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	Graphics gr(lpDrawItemStruct->hDC);
	// чтобы не было мерцания при отрисовке, создаем "сцену" в памяти с помощью переменной класса Bitmap
	Bitmap bmp((lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left + 1), (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top + 1), &gr);
	// создаем новый graphics, чтобы рисовать им в битмэпе, т.е. в памяти
	Graphics bmp_gr(&bmp);
	// заменяем все gr. на bmp_gr.

	bmp_gr.SetSmoothingMode(SmoothingModeAntiAlias);
	bmp_gr.Clear(Color::White);
	Pen black_pen(Color::Black, -1.0f);
	Pen gray_pen(Color::LightGray, -1.0f);
	Pen red_pen(Color::Red, -1.0f);
	Pen blue_pen(Color::DarkBlue, -1.0f);
	Pen green_pen(Color::LightGreen, -1.0f);
	// для строк
	FontFamily family(L"Arial");
	Gdiplus::Font myFont(&family, 10,FontStyleRegular,UnitPixel); // cемья, размер, отрисовка (обычная, жирная, курсив), отображение в пикселях
	SolidBrush blackBrush(Color(255, 0, 0, 0));
	StringFormat format;
	format.SetAlignment(StringAlignmentNear);
	
	
	

	float shirina = (lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left - 59); // Вычислили ширину окна в пикселях
	// вычли по 59 т.к. ширина в пикселях = то, что в скобках + 1, а мы хотим по 30 отнять с каждой стороны
	float alpha = shirina / (4 * M_PI / razn);

	float visota = (lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top -59); // -||- высоту окна в пикселях
	float beta = visota / 4.0f;





	Matrix matr; // Создаем матрицу преобразования (см. Лекцию 2). ОБЯЗАТЕЛЬНО ДО РИСОВАНИЯ!
	matr.Translate(30, 30);
	matr.Scale(alpha, -beta); // Вычисляем коэффициенты  для масштабирования; Пересчитывает мировые в пиксели
	matr.Translate(0, -2); // -||- переноса и отражения(относительно верхней границы окна); Действует в мировых координатах
	bmp_gr.SetTransform(&matr); // Непосредственно применяем матрицу


	float x1(0), y1(func(x1, fi)), y0 = func(0, 0); // начальные точки
	int kol = 200; // кол-во отрезков, на которые разбиваем промежуток 
	float step = (4 * M_PI / razn) / kol; // длина шага, на которые разбиваем

	for (int i = 1; i < kol; i++) // Цикл, в котором отрисовываем линии от точки к точке
	{
		float x2 = i*step;
		float y2 = func(x2, fi);
		bmp_gr.DrawLine(&blue_pen, x1, y1, x2, y2);
		x1 = x2;
		y1 = y2;
	}



	float x_step = (4 * M_PI / razn) / 10; // задаем шаг по икс для вертикальных линий сетки (для 10 линий)
	float y_step = 0.4; // -||- для игрек


	for (int i = 0; i < 11; i++)
	{
		float x = i*x_step;
		float y = i*y_step - 2; // чтобы не от нуля, а снизу рисовать
		bmp_gr.DrawLine(&gray_pen, x, -2.0f, x, 2.0f);
		bmp_gr.ResetTransform();
		PointF x_point(x-0.04, -2.1); // -0.1 чтобы чучуть влево переместить
		PointF y_point(-0.08, y+0.1);
		matr.TransformPoints(&x_point);
		matr.TransformPoints(&y_point);
		CString x_string;
		CString y_string;
		x_string.Format(L"%.2f",x);// (f тк float, значение, которое преобразуем), 2 - количество символов после запятой		
		bmp_gr.DrawString(x_string,-1,&myFont,x_point,&blackBrush); // -1 чтобы автоматически вычислялось количество символов
		y_string.Format(L"%.1f", y);
		bmp_gr.DrawString(y_string, -1, &myFont, y_point, &blackBrush);

		bmp_gr.SetTransform(&matr);
		bmp_gr.DrawLine(&gray_pen, 0.0f, y, (4 * M_PI / razn), y);
		
	}

	RectF BorderRect(0.0f, -2.0f, (4 * M_PI / razn), 4.0f);

	bmp_gr.DrawLine(&black_pen, 0.0f, 0.0f, (4 * M_PI / razn), 0.0f);
	bmp_gr.DrawLine(&black_pen, 0, -2, 0, 2);
	bmp_gr.DrawRectangle(&black_pen, BorderRect);

	gr.DrawImage(&bmp, 0, 0);
}

