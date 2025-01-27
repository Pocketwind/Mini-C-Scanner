#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>

#pragma warning (disable:4996)

#define NO_KEYWORDS 7
#define ID_LENGTH 12

enum tsymbol
{
	tnull = -1,
	tnot, tnotequ, tmod, tmodAssign, tident, tnumber,
	tand, tlparen, trparen, tmul, tmulAssign, tplus,
	tinc, taddAssign, tcomma, tminus, tdec, tsubAssign,
	tdiv, tdivAssign, tsemicolon, tless, tlesse, tassign,
	tequal, tgreat, tgreate, tlbracket, trbracket, teof,
	tconst, telse, tif, tint, treturn, tvoid,
	twhile, tlbrace, tor, trbrace
};

const char* symbol[] = {
	  "!", "!=", "%", "%=", NULL, NULL,
	  "&&", "(", ")", "*", "*=", "+",
	  "++", "+=", ",", "-", "--", "-=",
	  "/", "/=", ";", "<", "<=", "=",
	  "==", ">", ">=", "[", "]", "END OF FILE",
	  "const", "else", "if", "int", "return", "void",
	  "while", "{", "||", "}"
};

struct tokenType
{
	int number;
	union
	{
		char id[ID_LENGTH];
		int num;
	}value;
};

const char* keyword[NO_KEYWORDS] =
{
	"const","else","if","int","return","void","while"
};

enum tsymbol tnum[NO_KEYWORDS] =
{
	tconst,telse,tif,tint,treturn,tvoid,twhile
};

void lexicalError(int n)
{
	printf("Lexical Error : ");
	switch (n)
	{
	case 1:
		printf("식별자 길이는 %d미만\n", ID_LENGTH);
		break;
	case 2:
		printf("다음 문자는 &여야만함\n");
		break;
	case 3:
		printf("다음 문자는 |여아만함\n");
		break;
	case 4:
		printf("식별할수없는 문자\n");
		break;
	}
}

int superLetter(char ch)
{
	if (isalpha(ch) || ch == '_')
		return 1;
	else
		return 0;
}

int superLetterOrDigit(char ch)
{
	if (isalnum(ch) || ch == '_')
		return 1;
	else
		return 0;
}

int hexValue(char ch)
{
	switch (ch)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return (ch - '0');
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return (ch - 'A' + 10);
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return (ch - 'a' + 10);
	default:
		return -1;
	}
}

int getIntNum(char firstCharacter,FILE *file)
{
	int num = 0;
	int value;
	char ch;

	if (firstCharacter != '0')
	{
		ch = firstCharacter;
		do
		{
			num = 10 * num + (int)(ch - '0');
			ch = fgetc(file);
		} while (isdigit(ch));
	}
	else
	{
		ch = fgetc(file);
		if ((ch >= '0') && (ch <= '7'))
		{
			do
			{
				num = 8 * num + (int)(ch - '0');
				ch = fgetc(file);
			} while ((ch >= '0') && (ch <= '7'));
		}
		else if ((ch == 'X') || (ch == 'x'))
		{
			while ((value = hexValue(ch = fgetc(file))) != -1)
				num = 16 * num + value;
		}
		else num = 0;
	}
	ungetc(ch, stdin);
	return num;
}

struct tokenType scanner(FILE *file)
{
	struct tokenType token;
	int i, index;
	char ch, id[ID_LENGTH];
	token.number = tnull;
	do {
		while (isspace(ch = fgetc(file)));
		if (superLetter(ch)) {
			i = 0;
			do {
				if (i < ID_LENGTH) id[i++] = ch;
				ch = fgetc(file);
			} while (superLetterOrDigit(ch));
			if (i >= ID_LENGTH) lexicalError(i);
			id[i] = '\0';
			ungetc(ch, file);
			for (index = 0; index < NO_KEYWORDS; index++)
				if (!strcmp(id, keyword[index])) break;
			if (index < NO_KEYWORDS)
				token.number = tnum[index];
			else {
				token.number = tident;
				strcpy(token.value.id, id);
			}
		}
		else if (isdigit(ch)) {
			token.number = tnumber;
			token.value.num = getIntNum(ch, file);
		}
		else switch (ch) {
		case '/':
			ch = fgetc(file);
			if (ch == '*')
				do {
					while (ch != '*') ch = fgetc(file);
					ch = fgetc(file);
				} while (ch != '/');
			else if (ch == '/')
				while (fgetc(file) != '\n');
			else if (ch == '=') token.number = tdivAssign;
			else {
				token.number = tdiv;
				ungetc(ch, file);
			}
			break;
		case '!':
			ch = fgetc(file);
			if (ch == '=') token.number = tnotequ;
			else {
				token.number = tnot;
				ungetc(ch, file);
			}
			break;
		case '%':
			ch = fgetc(file);
			if (ch == '=')
				token.number = tmodAssign;
			else {
				token.number = tmod;
				ungetc(ch, file);
			}
			break;
		case '&':
			ch = fgetc(file);
			if (ch == '=')
				token.number = tand;
			else {
				lexicalError(2);
				ungetc(ch, file);
			}
			break;
		case '*':
			ch = fgetc(file);
			if (ch == '=') token.number = tmulAssign;
			else {
				token.number = tmul;
				ungetc(ch, file);
			}
			break;
		case '+':
			ch = fgetc(file);
			if (ch == '+') token.number = tinc;
			else if (ch == '=') token.number = taddAssign;
			else {
				token.number = tplus;
				ungetc(ch, file);
			}
			break;
		case '-':
			ch = fgetc(file);
			if (ch == '-') token.number = tdec;
			else if (ch == '=') token.number = tsubAssign;
			else {
				token.number = tminus;
				ungetc(ch, file);
			}
			break;
		case '<':
			ch = fgetc(file);
			if (ch == '=') token.number = tlesse;
			else {
				token.number = tless;
				ungetc(ch, file);
			}
			break;
		case '=':
			ch = fgetc(file);
			if (ch == '=') token.number = tequal;
			else {
				token.number = tassign;
				ungetc(ch, file);
			}
			break;
		case'>':
			ch = fgetc(file);
			if (ch == '=') token.number = tgreate;
			else {
				token.number = tgreat;
				ungetc(ch, file);
			}
			break;
		case '|':
			ch = fgetc(file);
			if (ch == '|') token.number = tor;
			else {
				lexicalError(3);
				ungetc(ch, file);
			}
			break;
		case '(': token.number = tlparen; break;
		case ')': token.number = trparen; break;
		case ',': token.number = tcomma; break;
		case ';': token.number = tsemicolon; break;
		case '[': token.number = tlbracket; break;
		case ']': token.number = trbracket; break;
		case '{': token.number = tlbrace; break;
		case '}': token.number = trbrace; break;
		case EOF: token.number = teof; break;
		default: {
			printf("current character : %c", ch);
			lexicalError(4);
			break;
		}
		}
	} while (token.number == tnull);
	return token;
}

int main(int argc,char **argv)
{
	if (argc != 2)
	{
		printf("파일명을 제대로 입력해주세요\n");
		return -1;
	}

	FILE* file;
	file = fopen(argv[1], "r");
	if (file == NULL)
	{
		printf("파일을 열수 없습니다.\n");
		return -1;
	}

	struct tokenType token;
	char ident[ID_LENGTH];

	while(feof(file)==NULL)
	{
		token = scanner(file);
		printf("Token -> ");

		if (token.number == 5)
		{
			printf("%d\t", token.value.num);
			printf(" : (%d, %d)\n", token.number, token.value.num);
		}
		else if (token.number == 4)
		{
			printf("%s\t", token.value.id);
			printf(" : (%d, %s)\n", token.number, token.value.id);
		}
		else
		{
			printf("%s\t", symbol[token.number]);
			printf(" : (%d, 0)\n", token.number);
		}
	}

	fclose(file);
	return 0;
}

