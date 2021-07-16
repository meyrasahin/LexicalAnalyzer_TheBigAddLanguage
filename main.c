#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

bool isKeyword(char* a) {
	if( strcmp("int", a) == 0 || strcmp("move", a) == 0 || strcmp("loop", a) == 0 || strcmp("to", a) == 0 || strcmp("times", a) == 0 || 
	strcmp("out", a) == 0 || strcmp("newline", a) == 0 || strcmp("add", a) == 0 || strcmp("sub", a) == 0|| strcmp("from", a) == 0) 
		return(true);
	else return(false);
}

bool isNumeric(char a){
	if(a == '0' || a == '1' || a == '2' || a == '3' || a == '4' || a == '5' || a == '6' || a == '7' || a == '8' || a == '9')
		return(true);
	else
		return(false);
}

bool isIntConstant(char* str){
	int size = strlen(str);
	int i;
	int j=0;
	
	if (size > 100){
		printf("%s\n", "An integer can be as big as 100 decimal digits.");
		return(false);
	}
	else{
		for (i = 0; i<size; i++)
			if(  (str[0] == '-' || isNumeric(str[0] ) ) && isNumeric(str[i]) ) 
				j++;
		
		if(j==size)
			return(true);
		else printf("%s\n", "Unrecognized character for integer constant.");
	}
	return(false);
}

bool isIdentifier(char* a) {
	if (strlen(a) > 20){
		printf("%s\n", "Variable names have a maximum length of 20 characters.");
		return(false);
	}
	
	if (!isalpha(a[0]))
		return(false);
		
	int i;
	bool ident = true;
	for(i=1; i<strlen(a); i++){
		if (ispunct(a[i]) != 0 && a[i] != '_' && a[strlen(a)-1] != '.' && a[strlen(a)-1] != ','){
			ident = false;	
		}		
		if (a[i] == '_')
			if (a[i+1] == '_'){	
				ident = false;
			}	
	}
	
	if(ident)
		return(true);
	else{
		printf("%s\n", "Unrecognized character.");
		return(false);
	}
}

bool isString(char* str){  
	if (strncmp(str, "\"", 1) == 0  || str == "\"" )
		return(true);
	else
		return(false);	
}

bool hasDot(char* token){
	if(strlen(token) == 1)
		return(false);

	int i;
	for(i=0; i<strlen(token)-1; i++){
		if(token[i] == '.')
			return(true);
	}
	return(false);
}

bool hasSeperator(char* token){
	if(strlen(token) == 1)
		return(false);
	
	int i;
	for(i=0; i<strlen(token)-1; i++){
		if(token[i] == ',')
			return(true);
	}
	return(false);
}

void lxAnalyzer(char* code){

	FILE *fw = fopen("myscript.lx", "w");
	
	char* lines[200];
	int i=0;
	
	char *token = strtok(code, "\n");
	
    while (token != NULL){
    	lines[i++] = token;	
    	token = strtok(NULL, "\n");
	}
	
	bool* inString = false;
	bool* inComment = false;
	bool* inBlock = false;
	char* words[500];
	
	bool* values[4];
	values[0] = inString;
	values[1] = inComment;
	values[2] = inBlock;
	
	int j = 0;
	int* k = 0;
	values[3] = k;

	while (j<i){
		char *line = lines[j];
		if (line[strlen(line)-1] == '\r')
			line[strlen(line)-1] = '\0';
	
		char *token = strtok(line, " ");

    	while (token != NULL){
 
    		if(hasSeperator(token)){
				char* first = strtok(token, ",");
				while(first!= NULL){
					wordAnalyzer(fw, first, inString, inComment, inBlock, words, k, values);
					first = strtok(NULL, ",");
					if (first != NULL)
						fprintf(fw, "Seperator\n");
				}	
			}
					
    		else if(hasDot(token)){
					char* first = strtok(token, ".");
					while(first!= NULL){
						wordAnalyzer(fw, first, inString, inComment, inBlock, words, k, values);
						fprintf(fw, "EndOfLine\n");
						first = strtok(NULL, ".");
					}			
			}
			
			else 
				wordAnalyzer(fw, token, inString, inComment, inBlock, words, k, values);
		
			inString = values[0];
			inComment = values[1];
			inBlock = values[2];
			k = values[3];
			token = strtok(NULL, " ");

    	}
		j++;
	}
	
	if (inString)
		printf("%s\n", "Missing Double Quote.");
	if (inComment)
		printf("%s\n", "Missing Curly Braces.");
	if (inBlock)
		printf("%s\n", "Missing Close Block.");
}

void wordAnalyzer(FILE* fw, char* token, bool* inString, bool* inComment, bool* inBlock, char* words[], int k, bool* values[]){	

 // We analyze the given token and determine what type of word it is using if else statements 
	if(isString(token)) {
		if(inComment)
			inComment = true;
		else{
			if (token[strlen(token)-1] == '\"' )
				fprintf(fw,"StringConstant '%s'\n", token);
			
			else if(token[strlen(token)-2] == '\"' ){
				if (token[strlen(token)-1] == '.' ){
					token[strlen(token)-1] = '\0';
					fprintf(fw,"StringConstant '%s'\n", token);
					fprintf(fw,"EndOfLine\n");
				}
				else if (token[strlen(token)-1] == ',' ){
					token[strlen(token)-1] = '\0';
					fprintf(fw,"StringConstant '%s'\n", token);
					fprintf(fw,"Seperator\n");
				}	
			}
			else {
				inString = true;
				words[k++] = token;	
			}
		}
	}
		
	else if(token[strlen(token)-1] == '\"' || token == "\""){
		words[k++] = token;	
		inString = false;
		int a;
		char string[k];
		string[0] = "";
		
		for (a=0; a<k; a++){
			strcat(string, words[a]);
			strcat(string, " ");
		}
		const char ch = '\"';
		char *new;
		new = strchr(string, ch);
			
		new[strlen(new)-1] = '\0';
		fprintf(fw,"StringConstant %s\n", new);
	}
			
	else if (token == "{" || token[0] == '{')
		inComment = true;
			
	else if (token == "}" || token[strlen(token)-1] == '}')
		inComment = false;
    		
    else if(token == ",") {
		if(inString)
    	words[k++] = token;
				
		else if(inComment)
			inComment = true;
				
		else fprintf(fw,"Seperator\n");
	}
    else if(token == ".") {
		if(inString)
    		words[k++] = token;
			
		else if(inComment)
			inComment = true;
				
		else fprintf(fw,"EndOfLine\n");
	}
	else if(token[strlen(token)-1] == '.' || token[strlen(token)-1] == ',' ){
    	if(inString)
    		words[k++] = token;
				
		else if(inComment)
			inComment = true;
				
		else{
    		char str[strlen(token)];
    		strcpy(str, token);
    		str[strlen(str)-1] = '\0';
    		
   			if(isKeyword(str))
				fprintf(fw,"Keyword '%s'\n", str);
					
   			else if(isIdentifier(str)) 
				fprintf(fw, "Identifier '%s'\n", str);
					
			else if(str == ",") 
				fprintf(fw,"Seperator\n");
					
			else if(isString(str)) 
				fprintf(fw,"StringConstant '%s'\n", str);
					
			else if(isIntConstant(str)) 
				fprintf(fw, "IntConstant '%s'\n", str);
					
			else if(str == ".") 
				fprintf(fw,"EndOfLine\n");
						
    		else if(str[0] == ('[')) {
    			inBlock = true;
    			fprintf(fw, "OpenBlock\n");
			}
					
			else if(str[0] == (']')) {
				inBlock = false;
				fprintf(fw, "CloseBlock\n");
			}
				
			else fprintf(fw, "Unrecognized character.\n");
			
			if (token[strlen(token)-1] == '.' )
				fprintf(fw,"EndOfLine\n");
					
   			else
				fprintf(fw,"Seperator\n");			
		}
	}
    		
    else if(isKeyword(token)) {
    	if(inString)
   		words[k++] = token;
				
		else if(inComment)
			inComment = true;
				
		else fprintf(fw,"Keyword '%s'\n", token);
	}
    else if(isIdentifier(token)) {
    	if(inString)
    		words[k++] = token;
				
		else if(inComment)
			inComment = true;
				
		else fprintf(fw, "Identifier '%s'\n", token);
	}
		
	else if(isIntConstant(token)) {
		if(inString)
    		words[k++] = token;
				
		else if(inComment)
			inComment = true;
				
		else fprintf(fw, "IntConstant '%s'\n", token);
	}
		
    else if(token[0] == ('[')) {
    	inBlock = true;
    	if(inString){
    		words[k++] = token;
		}
		else if(inComment){
			inComment = true;
		}
		else fprintf(fw, "OpenBlock\n");
	}
	else if(token[0] == (']')) {
		inBlock = false;
		if(inString)
    		words[k++] = token;
				
		else if(inComment)
			inComment = true;
				
		else fprintf(fw, "CloseBlock\n");
	}
	
	else fprintf(fw, "Unrecognized character.\n");
			
	values[0] = inString;
	values[1] = inComment;
	values[2] = inBlock;
	values[3] = k; 
			
}

int main() {
	FILE *file;
	long fileSize;
	char *code;
	
	char fileName[50];
    printf("Enter the file name: ");
    gets(fileName);  

	file = fopen(fileName, "rb");
	if (file == NULL) {
		perror("myscript.ba");
		exit(1);
	}
	
	fseek(file, 0L, SEEK_END);
	fileSize = ftell(file);
	rewind(file);

	code = (char*)calloc(1, fileSize + 1);

	if (1 != fread(code, fileSize, 1, file)) {
		fclose(file);
		free(code);
		exit(1);
	}

	lxAnalyzer(code);
	
	system("pause");
	return 0;
}
