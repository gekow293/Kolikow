#pragma once


// DrawPicture

class DrawPicture : public CStatic
{
	DECLARE_DYNAMIC(DrawPicture)

public:
	DrawPicture();
	virtual ~DrawPicture();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	
};


