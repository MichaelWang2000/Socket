#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ProcessData.h"
#include "checksum.h"
#include "AES.h"

#define DATA_BUFFER 1024

void _itoa(long i, char *string)
{
	int power = 0, j = 0;
 
	j = i;
	for (power = 1; j>10; j /= 10)
		power *= 10;
 
	for (; power>0; power /= 10)
	{
		*string++ = '0' + i / power;
		i %= power;
	}
	*string = '\0';
	printf("%s\n", string);
}

char* DataProcessing(char* data, int dataLength)
{
	char operation[17];
	char checksum[17];
	char password[33];
	char length[33];
	char dataArea[DATA_BUFFER];

	int iterator = 0;
	/*Read the first 16 bit data*/
	for (iterator; iterator < 16; iterator++)
	{
		operation[iterator] = data[iterator];
	}
	operation[16] = '\0';

	/*Read the next 16 bit data*/
	for (int j = 0; iterator < 32; iterator++, j++)
	{
		checksum[j] = data[iterator];
	}
	checksum[16] = '\0';

	/*Read the next 32 bit data*/
	for (int j = 0; iterator < 64; iterator++, j++)
	{
		length[j] = data[iterator];
	}
	length[32] = '\0';

	/*Read the next 32 bit data*/
	for (int j = 0; iterator < 96; iterator++, j++)
	{
		password[j] = data[iterator];
	}
	password[32] = '\0';

	/*Read the remain data*/
	for (int j = 0; iterator < dataLength; iterator++, j++)
	{
		dataArea[j] = data[iterator];
	}
	dataArea[dataLength - 96] = '\0';

	/*Start to process each part of data*/
	/*Start to calculate the checksum*/
	char* chekcsumResult;
	chekcsumResult = calculateChecksum(operation, length, password, dataArea);
	
	char readPointer[17];
	for (int i = 0; i < 16; i++)
	{
		readPointer[i] = *(chekcsumResult + i);
	}

	/*Check whether the checksum code is totally match or not*/
	for (int i = 0; i < 16; i++)
	{
		if (readPointer[i] != checksum[i])
		{
			char errorMessage[] = "Error code: 301 Checksum dismatch!";// the checksum is not totally match
			return errorMessage;
		}
	}
	
	/*Start to process the operator*/
	int operationInt = 0;
	for (int bitNumber = 0; bitNumber < 16; bitNumber++)
	{
		operationInt += (operation[bitNumber] - '0') * pow(2, 15 - bitNumber);
	}

	/*Start to process the operator*/
	int passwordInt = 0;
	char passwordString[1024];
	for (int bitNumber = 0; bitNumber < 32; bitNumber++)
	{
		passwordInt += (password[bitNumber] - '0') * pow(2, 31 - bitNumber);
	}
	_itoa(passwordInt, passwordString);

	/*Start to procee the file content*/
	char fileContent[DATA_BUFFER];
	int j = 0;
	int totalLength = strlen(dataArea) / 8;
	for (int times = 0; times < totalLength; times++)
	{
		int groupSum = 0;
		for (int i = 0; i < 8; i++, j++)
		{
			groupSum += (dataArea[j] - '0') * pow(2,7 - i);
		}
		fileContent[times] = (char)groupSum;
	}
	fileContent[totalLength] = '\0';

	// cout << "operator: " << operationInt << endl;
	// cout << "checksum: " << checksum << endl;
	// cout << "length: " << length << endl;
	// cout << "Data Area: " << fileContent << endl;
	// cout << "Password: " << passwordString << endl;

	if (operationInt == 100) // encrypt
	{
		char key[17];
		int encryptedDataLength;
		char encryptedData[1024] = "";
		char finalResult[1024];
		/* First check whether the password is 16 byte or not */
		if (strlen(passwordString) > 16)
		{
			char errorMessage[] = "Length of the password must less or equal to 16 byte!";
			return errorMessage;
		}
		else if (strlen(passwordString) == 16)
		{
			//strcpy(key, passwordString);
			encryptedDataLength = AES_encrypt(fileContent, strlen(fileContent), encryptedData, passwordString);
			for (int i = 0, j = 0; i < encryptedDataLength; ++i, j += 2)
			{
				sprintf(finalResult + j, "%02X", (unsigned char)encryptedData[i]);
			}
			for (int i = 0, j = 0; i < encryptedDataLength; ++i, j += 2) {
				printf("%02X", (unsigned char)encryptedData[i]);
			}
			/*Then, send the object data*/
			return finalResult;
		}
		else
		{
			memset(key, '0', 17);
			/* We need to expand the initial array to 16 byte */
			for (int pointer = 16 - strlen(passwordString), j = 0; pointer != 16; pointer++, j++)
			{
				key[pointer] = passwordString[j];
			}
			key[16] = '\0';
			// strcpy(key, passwordString);
			encryptedDataLength = AES_encrypt(fileContent, strlen(fileContent), encryptedData, key);
			for (int i = 0, j = 0; i < encryptedDataLength; ++i, j += 2)
			{
				sprintf(finalResult + j, "%02X", (unsigned char)encryptedData[i]);
			}
			/*Then, send the object data*/
			return finalResult;
		}

	}
	
	else if (operationInt == 200) // decrypt
	{
		/* Code to decript */
		char key[17];
		int decryptedDataLength;
		char decryptedData[1024] = "";
		char needToDecryptedData[1024] = "";
		int z = 0;
		/* Need to transform the filecontent to unsigned char array */
		for (int i = 0; i < strlen(fileContent); z++)
		{
			int groupsum = 0;
			for (int j = 0; j < 2; i++, j++)
			{
				fileContent[i] = toupper(fileContent[i]);
				if ('0' <= fileContent[i] && fileContent[i] <= '9')
				{
					groupsum += (fileContent[i] - '0') * pow(16, 1 - j);
				}
				else if ('A' <= fileContent[i] && fileContent[i] <= 'Z')
				{
					groupsum += (fileContent[i] - 'A' + 10) * pow(16, 1 - j);
				}
			}
			needToDecryptedData[z] = (unsigned char)groupsum;
		}
		/* First check whether the password is 16 byte or not */
		if (strlen(passwordString) > 16)
		{
			char errorMessage[] = "Length of the password must less or equal to 16 byte!";
			return errorMessage;
		}
		else if (strlen(passwordString) == 16)
		{
			strcpy(key, passwordString);
			decryptedDataLength = AES_decrypt(needToDecryptedData, z, decryptedData, key);
			return decryptedData;
		}
		else
		{
			memset(key, '0', 17);
			/* We need to expand the initial array to 16 byte */
			for (int pointer = 16 - strlen(passwordString), j = 0; pointer != 16; pointer++, j++)
			{
				key[pointer] = passwordString[j];
			}
			key[16] = '\0';
			decryptedDataLength = AES_decrypt(needToDecryptedData, z, decryptedData, key);
			return decryptedData;
		}
	}
	
	else // error return 301 
	{
		/* Error code 301 */
		char errorMessage[] = "Error code : 301 Unknown operation!";
		return errorMessage;
	}
}