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

void createTrophy()
{
	LCD_WriteData(0x15);
	LCD_WriteData(0x1f);
	LCD_WriteData(0xe);
	LCD_WriteData(0xe);
	LCD_WriteData(0x4);
	LCD_WriteData(0x4);
	LCD_WriteData(0xe);
	LCD_WriteData(0x1f);
}

void createDisruption_One()
{
	LCD_WriteData(0x18);
	LCD_WriteData(0x18);
	LCD_WriteData(0x3);
	LCD_WriteData(0x3);
	LCD_WriteData(0x18);
	LCD_WriteData(0x18);
	LCD_WriteData(0x3);
	LCD_WriteData(0x3);
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
	LCD_WriteData(0x4);
	LCD_WriteData(0x4);
	LCD_WriteData(0xa);
	LCD_WriteData(0x11);
	LCD_WriteData(0x11);
	LCD_WriteData(0xa);
	LCD_WriteData(0x4);
	LCD_WriteData(0x4);
}

void createDisruption_Four()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0xa);
	LCD_WriteData(0x4);
	LCD_WriteData(0x4);
	LCD_WriteData(0x4);
	LCD_WriteData(0x4);
	LCD_WriteData(0xa);
	LCD_WriteData(0x0);
}

void createDisruption_Five()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0xe);
	LCD_WriteData(0x8);
	LCD_WriteData(0x4);
	LCD_WriteData(0x2);
	LCD_WriteData(0xe);
	LCD_WriteData(0x4);
	LCD_WriteData(0x0);
}

void createDisruption_Six()
{
	LCD_WriteData(0x0);
	LCD_WriteData(0xa);
	LCD_WriteData(0x2);
	LCD_WriteData(0xe);
	LCD_WriteData(0x8);
	LCD_WriteData(0xa);
	LCD_WriteData(0xa);
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
	createTrophy();
	createBracket();
	createDisruption_One();
	createDisruption_Two();
	createDisruption_Three();
	createDisruption_Four();
	createDisruption_Five();
	createDisruption_Six();
}

#endif