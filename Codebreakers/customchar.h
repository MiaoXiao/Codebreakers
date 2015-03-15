#ifndef CUSTOM_CHAR_H
#define CUSTOM_CHAR_H

void createBlock()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
}

void createWinLogo()
{
	//0x0,0x1,0x3,0x16,0x1c,0x8,0x0,0x0
	LCD_WriteData(0x0);
	LCD_WriteData(0x1);
	LCD_WriteData(0x3);
	LCD_WriteData(0x16);
	LCD_WriteData(0x1c);
	LCD_WriteData(0x8);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
}

void createLoseLogo()
{
	//0x0,0x1b,0xe,0x4,0xe,0x1b,0x0,0x0
	LCD_WriteData(0x0);
	LCD_WriteData(0x1b);
	LCD_WriteData(0xe);
	LCD_WriteData(0x4);
	LCD_WriteData(0xe);
	LCD_WriteData(0x1b);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
}

void createDisruption_One()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0xc);
	LCD_WriteData(0xc);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
	LCD_WriteData(0x6);
	LCD_WriteData(0x6);
	LCD_WriteData(0x0);
}

void createDisruption_Two()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0x6);
	LCD_WriteData(0x6);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0xc);
	LCD_WriteData(0xc);
	LCD_WriteData(0x0);
}

void createDisruption_Three()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0x0);
}

void createDisruption_Four()
{
	//0x0,0x0,0xe,0xe,0xe,0x0,0x0,0x0
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
	LCD_WriteData(0x0);
}

void createDisruption_Five()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0x2);
	LCD_WriteData(0x6);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0x6);
	LCD_WriteData(0x2);
	LCD_WriteData(0x0);
}

void createDisruption_Six()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0x8);
	LCD_WriteData(0xc);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0xc);
	LCD_WriteData(0x8);
	LCD_WriteData(0x0);
}

void createBracket()
{
	LCD_WriteData(0xf);
	LCD_WriteData(0x18);
	LCD_WriteData(0x1c);
	LCD_WriteData(0x6);
	LCD_WriteData(0x6);
	LCD_WriteData(0x1c);
	LCD_WriteData(0x18);
	LCD_WriteData(0xf);
}


void initCustomChar()
{
	LCD_WriteCommand(0x40);
	createBlock();
	createWinLogo();
	createLoseLogo();
	//createBracket();
	createDisruption_One();
	createDisruption_Two();
	createDisruption_Three();
	createDisruption_Four();
	createDisruption_Five();
	createDisruption_Six();
}

#endif