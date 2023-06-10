#include <stdio.h>
#include <string.h>
#include "AES.h"
#include"checksum.h"

char* add(char operand1[], char operand2[])
{
	char result1[17];
	memset(result1, '0', 16);
	result1[16] = '\0';
	int carry = 0;
	for (int i = 15; i >= 0; i--)
	{
		if ((operand1[i] - '0' + operand2[i] - '0' + carry) > 1)
		{
			result1[i] = '0';
			carry = 1;
		}
		else
		{
			if ((operand1[i] - '0' + operand2[i] - '0' + carry) == 1)
			{
				result1[i] = '1';
			}
			else if ((operand1[i] - '0' + operand2[i] - '0' + carry) == 0)
			{
				result1[i] = '0';
			}
			carry = 0;
		}
	}
	if (carry == 1)
	{
		char cycal[17] = "0000000000000001";
		return add(result1, cycal);
	}
	else if (carry == 0)
	{
		return result1;
	}
}

char* calculateChecksum(char operation[], char length[], char password[], char data[])
{
	char segment1[16];
	char segment2[16];
	char segment3[16];
	char segment4[16];
	char segment6[16];
	int i;
	for (i = 0; i < 16; i++)
	{
		segment1[i] = length[i];
		segment3[i] = password[i];
	}
	for (int j = 0; j < 16; j++, i++)
	{
		segment2[j] = length[i];
		segment4[j] = password[i];
	}
	int abc = strlen(data);
	char* result1;
	result1 = add(segment1, segment2);
	char asd[17];
	for (int i = 0; i < 16; i++)
	{
		asd[i] = *(result1 + i);
	}

	result1 = add(asd, segment3);
	for (int i = 0; i < 16; i++)
	{
		asd[i] = *(result1 + i);
	}

	result1 = add(asd, segment4);
	for (int i = 0; i < 16; i++)
	{
		asd[i] = *(result1 + i);
	}
	for (i = 0; i < 16; i++)
	{
		segment6[i] = operation[i];
	}

	result1 = add(asd, segment6);
	for (int i = 0; i < 16; i++)
	{
		asd[i] = *(result1 + i);
	}

	int divide = abc / 16;
	int remainder = abc % 16;
	for (int count = 0; count < divide; count++)
	{
		char segment5[16];
		for (int gai = 0; gai < 16; gai++)
		{
			segment5[gai] = data[count * 16 + gai];
		}
		result1 = add(asd, segment5);
		for (int i = 0; i < 16; i++)
		{
			asd[i] = *(result1 + i);
		}
	}
	char aaa[16];
	memset(aaa, '0', 16);
	for (int a = 0; a < remainder; a++)
	{
		aaa[a] = data[divide * 16 + a];
	}
	char* re = add(asd, aaa);

	for (int i = 0; i < 16; i++)
	{
		if (*(re + i) == '0')
			*(re + i) = '1';
		else if (*(re + i) == '1')
			*(re + i) = '0';

		asd[i] = *(re + i);
	}
	return asd;
}