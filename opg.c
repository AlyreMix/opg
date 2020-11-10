#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define Undefined 12138

typedef struct Stack {
	int bottom;
    int top;
	char sign[1024];
};

typedef struct Sentence {
	int ptr;
	int len;
	char content[1024];
};

struct Stack stack;
struct Stack tmpStack;
struct Sentence str;
const int priorityMatrix[6][6] = {
	{1, -1, -1, 1, -1, 1},
	{1, 1, -1, 1, -1, 1},
	{-1, -1, -1, 0, -1, Undefined},
	{1, 1, Undefined, 1, Undefined, 1},
	{1, 1, Undefined, 1, Undefined, 1},
	{-1, -1, -1, -1, -1, Undefined}
};

void init()
{
	stack.bottom = 0;
	stack.top = -1;
	memset(stack.sign, 0, sizeof(stack.sign));
	tmpStack.bottom = 0;
	tmpStack.top = -1;
	memset(tmpStack.sign, 0, sizeof(tmpStack.sign));
	str.ptr = 0;
	str.len = strlen(str.content);
	str.content[str.len] = '#';
}

void DoReduce()
{
	printf("R\n");
}

void REException()
{
	printf("RE\n");
}

void EException()
{
	printf("E\n");
}

bool isVt(char c)
{
	switch (c) 
	{
	case '+':
        return true;
        break;
	case '*':
        return true;
        break;
	case '(':
        return true;
        break;
	case ')':
        return true;
        break;
	case 'i':
        return true;
        break;
	case '#':
        return true;
        break;
	default:
        return false;
	}
}

bool isVn(char c)
{
    if(c == 'N')
    {
        return true;
    }
    return false;
}

int findIndex(char vt)
{
	switch (vt)
	{
	case '+':
        return 0;
        break;
	case '*':
        return 1;
        break;
	case '(':
        return 2;
        break;
	case ')':
        return 3;
        break;
	case 'i':
        return 4;
        break;
	case '#':
        return 5;
        break;
	default:
        EException(); 
        return -1;
	}
}

void stackpush(char c)
{
	stack.top++;
	stack.sign[stack.top] = c;
}

void read(char c)
{
	printf("I%c\n", c);
	stackpush(c);
}

int stacklen()
{
    int len;
	len = stack.top - stack.bottom + 1;
    return len;
}

bool expect(char src, char dst)
{
	if (src == dst)
    {
        return true;
    }
	return false;
}

char stackpop()
{
    int index = stack.top;
	if (stacklen() <= 0)
	{
		return '\0';
	}
	stack.top--;
	return stack.sign[index];
}

void tmpStackpush()
{
    tmpStack.top ++;
	tmpStack.sign[tmpStack.top] = stack.sign[stack.top];
    stack.top --;
}

void tmpStackpop()
{
    stack.top ++;
	stack.sign[stack.top] = tmpStack.sign[tmpStack.top];
    tmpStack.top --;
}

char stackgetVt()
{
	for (int i = stack.top; i >= stack.bottom; i--)
	{
		if (isVt(stack.sign[i]))
			return stack.sign[i];
	}
	return '#';
}

int stackgetNextVtIndex(int src)
{
	for (int i = src; i >= stack.bottom; i--)
	{
		if (isVt(stack.sign[i]))
			return i;
	}
	return -1;
}

bool reduceLPrime(int left, int right)
{
	if (left > right)
		return false;
	if (right - left == 2)
	{
        char a = stack.sign[left];
        char b = stack.sign[left + 1];
        char c = stack.sign[left + 2];
		if (a == 'N' && (b == '+' || b == '*') && c == 'N')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			for (int i = left; i <= right; i++)
				stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
		else if (a == '(' && b == 'N' && c == ')')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			for (int i = left; i <= right; i++)
				stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
	}
	else if (right == left)
	{
		if (stack.sign[left] == 'i')
		{
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpush();
			stackpop();
			stackpush('N');
			for (int i = right + 1; i <= stack.top; i++)
				tmpStackpop();
			return true;
		}
	}
	return false;
}

bool findLPrime()
{
	char formerSign = '#';
	char currentSign = '#';
	int src = 0;
	int dst = 0;
	for (int i = stack.bottom; i <= stack.top; i++)
	{
		if (isVt(stack.sign[i]))
		{
			dst = i;
			currentSign = stack.sign[i];
			if (priorityMatrix[findIndex(formerSign)][findIndex(currentSign)] > 0)
			{
				if (src >= 1 && isVn(stack.sign[src - 1]))
					src--;
				if (dst <= stack.top - 1 && isVn(stack.sign[dst + 1]))
					dst++;
				return reduceLPrime(src, dst);
			}
			else if (priorityMatrix[findIndex(formerSign)][findIndex(currentSign)] < 0)
			{
				src = i;
				formerSign = currentSign;
			}
		}
	}
	if (src >= 1 && isVn(stack.sign[src - 1]))
		src--;
	if (dst <= stack.top - 1 && isVn(stack.sign[dst + 1]))
		dst++;
	return reduceLPrime(src, dst);
}

void analyseGrammar() 
{
	if (strlen(str.content) <= 0)
	{
		EException();
		return;
	}

	init();

	while (str.ptr < str.len || stacklen()>1 || stack.sign[stack.top] != 'N')
	{
		if (!isVt(str.content[str.ptr]))
		{
			EException();
			return;
		}
		else if (priorityMatrix[findIndex(stackgetVt())][findIndex(str.content[str.ptr])] == Undefined)
		{
			EException();
			return;
		}
		else if (priorityMatrix[findIndex(stackgetVt())][findIndex(str.content[str.ptr])] <= 0)
		{
			read(str.content[str.ptr++]);
		}
		else
		{
			if (findLPrime())
			{
				printf("R\n");
			}
			else
			{
				REException();
				return;
			}
		}
	}
	return;
}

int main()
{
	scanf("%s", &str.content);
	analyseGrammar();
	return 0;
}
