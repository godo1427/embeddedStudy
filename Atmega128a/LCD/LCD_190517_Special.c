/*
 * LCD_190517_Special.c
 *
 * Created: 5/17/2019 2:44:05 PM
 * Author : tlgus
 */ 

#include <avr/io.h>
#define F_CPU 16000000
#include <util/delay.h>

#define FUNCSET 0x28 //function set, ������ �� 4��Ʈ, 2�ٷ� ǥ��, 5X7��Ʈ ���
#define ENTMODE 0x06 //entry mode set, LCDǥ��â�� ���ڸ� ǥ���ϰ� Ŀ���� ���������� �̵�
#define ALLCLR 0x01 //all clear, LCDǥ��â�� Ŭ�����Ű�� Ŀ���� ù ���� ù ĭ�� ��ġ
#define DISPON 0x0c //display on, ǥ�� on

void LCD_INIT(void);
void COMMAND(unsigned char byte);
void DATA(unsigned char byte);
void STRING(unsigned char font[], unsigned char n);
void MOVE(int y, int x);



int main(void)
{
	int min = 0, sec = 0, ms = 0;
	int mode = 0;
	/* Replace with your application code */
	unsigned char string_M[] = "TIME";
	unsigned char string_L[] = "00:00:00";
	DDRB = 0xFF;
	PORTB = 0x00;
	DDRE = 0x00;
	DDRF = 0xFF;
	
	LCD_INIT();
	
	MOVE(1, 7);
	STRING(string_M, 4);
	MOVE(2, 5);
	STRING(string_L, 8);
	
	while (1)
	{
		if(!((PINE & (1<<PINE0))))
		{
			mode = 1;
			
			while(!((PINE&0x01)));
			_delay_ms(30);
		}
		if(!(PINE & 0x02))
		{
			min = 0, sec = 0, ms = 0;
			MOVE(2, 5);
			sprintf(string_L, "%02d:%02d:%02d", min, sec, ms);
			STRING(string_L, 8);
			while (!(PINE &0x02));
		}
		
		while (mode == 1)
		{
			_delay_ms(10);
			ms++;
			if(ms==100)
			{
				ms = 0;
				sec++;
				if(sec == 60)
				{
					sec = 0;
					min++;
				}
			}
			if(!(PINE&0x01))
			{
				mode = 0;
				while(!(PINE&0x01));
			}
			
			
			MOVE(2, 5);
			sprintf(string_L, "%02d:%02d:%02d", min, sec, ms);
			STRING(string_L, 8);
		}
				
	}
}

void COMMAND(unsigned char byte)
{
	_delay_ms(2);

	PORTB = byte & 0xF0;	//���ɾ� ����
	PORTB &= 0b11111100;	//RS = 0, RW = 0
	_delay_us(1);			//RS & RW setup
	
	PORTB |= 0b00000100;	//E = 1, lcd����
	_delay_us(1);			//E pulse width time
	PORTB &= 0b11111011;	//E = 0
	
	PORTB = (byte<<4) & 0xF0; //���ɾ� ����
	PORTB &= 0b11111100;
	_delay_us(1);
	
	PORTB |= 0b00000100;  //E = 1, lcd����
	_delay_us(1);
	PORTB &= 0b11111011;  //E = 0
}

void MOVE(int y, int x)
{
	unsigned char data = 0;
	if(y==1)
	{
		data = 0x80 + x - 1;
	}
	else if(y == 2)
	{
		data = 0xC0 + x - 1;
	}
	
	COMMAND(data);
}
void STRING(unsigned char font[], unsigned char n)
{
	_delay_ms(2);
	int i = 0;
	for(;n>0;n--)
	{
		PORTB = font[i] & 0xf0;//���� 4��Ʈ ���ɾ� ����
		PORTB |= 0b00000001;//RS = 1, ������ ����
		PORTB &= 0b11111101;//RW = 0, ���� ����
		_delay_us(1);
		PORTB |= 0b00000100;//E = 1, lcd ����
		_delay_us(1);
		PORTB &= 0b11111011;//E = 0
		
		PORTB = (font[i]<<4) & 0xF0;	 //���� 4��Ʈ ������ ����
		PORTB |= 0b00000001;		//RS = 1, ������ ����
		PORTB &= 0b11111101;		//RW  = 0, ���� ����
		_delay_us(1);
		PORTB |= 0b00000100;//E = 1, lcd ����
		_delay_us(1);
		PORTB &= 0b11111011;//E = 0;
		
		_delay_us(40);
		i++;
	}
}
void DATA(unsigned char byte)
{
	_delay_ms(2);
	
	PORTB = byte & 0xf0;//���� 4��Ʈ ���ɾ� ����
	PORTB |= 0b00000001;//RS = 1, ������ ����
	PORTB &= 0b11111101;//RW = 0, ���� ����
	_delay_us(1);
	PORTB |= 0b00000100;//E = 1, lcd ����
	_delay_us(1);
	PORTB &= 0b11111011;//E = 0
	
	PORTB = (byte<<4) & 0xF0;	 //���� 4��Ʈ ������ ����
	PORTB |= 0b00000001;		//RS = 1, ������ ����
	PORTB &= 0b11111101;		//RW  = 0, ���� ����
	_delay_us(1);
	PORTB |= 0b00000100;//E = 1, lcd ����
	_delay_us(1);
	PORTB &= 0b11111011;//E = 0;
}



void LCD_INIT(void)
{
	_delay_ms(30);		//���� ���� �� 30ms �̻� ����
	
	//Function set
	//�������̽�(DL) = 0(4bit), ����(N) = 1(2����), ��Ʈ(F) = 0(5*8 dot)
	COMMAND(FUNCSET);
	_delay_us(40);

	//Display ON/OFF Control
	//ȭ�� ǥ��(D) = 1(on), Ŀ��(C) = 0(off), ����ũ(B) = 0(off)
	COMMAND(DISPON);
	_delay_us(40);

	//Clear Display
	//ȭ�� Ŭ����, Ŀ�� Ȩ��ġ
	COMMAND(ALLCLR);
	_delay_ms(1.53);
	
	//Entry Mode Set
	//Ŀ�� ����(I/D) = 1(address����), ǥ���̵�(S)=0(�̵����� ����
	COMMAND(ENTMODE);
	_delay_us(40);
	
}
