#include <stdio.h>
#include <stdlib.h>
#include "math.h"
#include "string.h"
//#include "vld.h"

/*
(a) Allow more than two outcomes/classes
(b) Allow continuous-valued attributes
(c) Allow printing the tree
(d) Allow the option of rule post-pruning and printing
the rules
(e) Two data sets: Tennis and Iris on the course
web site.
(f) The same program should be able to handle the
two data sets.
*/

#define FIND_IRRELEVANT 0 
#define FIND_CONTRADICTION 0 

#define NUM_ATTR_VALUE  10 // more than 'no' and 'yes', maybe the third value exists.
#define NUM_OUTCOME  5 // the outcome can have 5 different values at most. it can be modified,
// but need to be certain at the beiginning. because it will represent the size
//  of an outcome[]. the type of the outcome is not sure, maybe y/n, 1/0,
// A/B/C etc.

typedef enum
{
	ATTRI_TYPE,
	VALUE_TYPE,
	ALL_ATTRIBUTES_TYPE
}EXAMPLE_TYPE;

typedef enum
{
	FILE_ATTRIBUTE,
	FILE_TRAINING
}FILE_TYPE;
/*
typedef enum
{
ALL_POSITIVE,
ALL_NEGATIVE,
NEITHER_P_N
}OUTCOME_STATE;
*/

typedef enum
{
	FALSE,
	TRUE
}BOOL;

typedef enum
{
	SAME_OUTCOME,
	DIFF_OUTCOME,
	NEITHER_P_N
}OUTCOME_STATE;

typedef enum
{
	DISCRETE_VAL_ATTR,
	CONTINUOUS_VAL_ATTR
}ATTRIBUTE_TYPE;

typedef struct
{
	char val_name[50];                       // when the value is discrete, the val_name will be the real value
											 // but if the value is continuous, the val_name will be recorded with "T/F"
	int outcome[NUM_OUTCOME];  // number of every type of outcome for every value
	int  outcome_type_num;           //the total amount of types of outcome, if "Yes"/"No", the value is 2

}Value;

typedef struct
{
	char attr_name[50];
	Value value[NUM_ATTR_VALUE];
	int value_type_num;
	ATTRIBUTE_TYPE attr_type;

}Attribute;

typedef struct
{
	char node_name[50];
	char * branches_value[10];    // these two arrays must have one-to-one correspondence with the same index
								  // it records value of every branch
	int branch_num;                     // the number of branches belong to one attribute(one node)
	struct DecisionTree *branches[10];
	int true_false_num[2];  // 1 is yes, 2 is false
}DecisionTree;

typedef struct
{
	char name[50];
	char value[50];
	//char *outcome;
	//char *accuracy;
	struct Rule *nextRule;
	struct Rule *prevRule;
	int level;
	int true_false_num[2];
}Rule;

typedef struct
{
	char *value_name[50];
	int value_class[50][3];
	int name_amount;
	int class_amount;
}Values; //used for do statistic

Attribute examples[1000];
int num_examples = 0;   // store the total number of training data
int attribute_amount = 0; // store the total number of attributes in training data
char *targetAttr_majority;  // store the majority of target attribute
							//compute the proportion of every kind of outcome while reading the file
int training_Outcome_Proportion[NUM_OUTCOME];
int attr_remove_index[10] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1 }; // store the index of being removed when using ID3
															   //int attr_remove_count = 0;   // +1 when remove an attribute
int outcome_type_amount = 0;
char *outcome_string[10];   // for example, it has two or more elements which are YES , NO. 10 means this tree might have at most 10 types of outcome
Rule *rule_array[20];
int indexOfIrreAttr[10];
char *IrreAttr[10];
int amountOfIrreAttr = 0;
char * deleteAttribute(Attribute exp[], int index);

char * str_combine(char *a, char *b);
int initExample(char *buf[200][200], FILE_TYPE type);
char * deleteAttribute(Attribute exp[], int index);
void split(char *str);
int isRemoved(index);
void cmpAndDelete(char *buf[200][200], int num_of_example);
void findClassError(char *buf[200][200], int num_of_example, int store[50][50]);
int isIrrelevantAttr(char *buf[200][200], int num_of_example, int remove_index);
int readFile(char *path, char *buf[200][200], FILE_TYPE file_type);
float calEntropy(Attribute attribute, char *val_name, EXAMPLE_TYPE type);
int calValueAmount_name(char *val_name);
char *cmpOutcome(char *val_name);
int calValueAmount_index(Attribute attr, int val_index);
void findAttributeNValueIndex(char *val_name, int *attr_idx, int *v_idx);
int findAttribute(char *val_name, Attribute *attr);
int calExampleAmount(int index);
float calInfoGain(char *example_name, char *estimated_attr_name, EXAMPLE_TYPE type);
OUTCOME_STATE searchExmpOutState(char *example_name, char *most_common_output);
BOOL checkAttrAvailable(int attr_remove[], int index);
void ID3(char *example_name, DecisionTree *root, EXAMPLE_TYPE type, int *attr_remove_idx, int attr_remove);
void printDesicionTree(DecisionTree *root, int blk);
void printDesicionTreeByLine(DecisionTree *root, char _name[10][50], int print_count);
void createRules(DecisionTree *tree, Rule *preRule, Rule *rule);
void postPruning(char *buf[200][200]);
void printRule(Rule *r[]);
void changeLine(char *buf[200][200], int row_des, int row_source, int col_num);
void genContiAttr(char *buf[200][200]);
void calContiValOutcome(float threshold[100], int amount, char *buf[200][200], int attr_index, char attr_name[15][10]);
int findOutcomeIndex(char *name);
void calAccuracy_rule(char *buf[200][200], Rule *r);
void calAccuracy_tree(char *buf[200][200], DecisionTree *root);







char * str_combine(char *a, char *b)
{
	char *ptr;
	int lena = strlen(a);
	int lenb = strlen(b);
	int i, l = 0;
	ptr = (char *)malloc((lena + lenb + 1) * sizeof(char));
	for (i = 0; i<lena; i++)
		ptr[l++] = a[i];

	for (i = 0; i<lenb; i++)
		ptr[l++] = b[i];

	ptr[l] = '\0';
	return(ptr);
}

int initExample(char *buf[200][200], FILE_TYPE type)
{

	if (type == FILE_ATTRIBUTE)
	{
		outcome_type_amount = atoi(buf[attribute_amount][0]) - 1;  // attribute_amount + 2 is the total number of the file, but index starts with 0 and there is a empty line;

		int i, j;

		for (i = 0; i < attribute_amount; i++)
		{

			int num = atoi(buf[i][0]);
			for (j = 2; j < num + 1; j++)  //buf[i][1] is attribute, after that, they are all values;
			{

				strcpy(examples[i].attr_name, buf[i][1]);
				strcpy(examples[i].value[j - 2].val_name, buf[i][j]);
				examples[i].value_type_num = num - 1;
				examples[i].value[j - 2].outcome_type_num = outcome_type_amount;
				//printf("qqq1: %s\n",examples[i].attr_name);
				//printf("qqq2: %s\n",examples[i].value[j-2].val_name);

				//printf("qqq3: %d\n",examples[i].value_type_num);

				//printf("qqq4: %d\n",examples[i].value[j-2].outcome_type_num);

			}

		}
		int m;
		for (m = 2; m < outcome_type_amount + 2; m++)
		{
			outcome_string[m - 2] = buf[attribute_amount][m];
		}

	}
	else if (type == FILE_TRAINING)
	{
		int m, n, p, e;
		char *val_name;
		int outcome_amount = 0;
		Attribute attr;
		for (m = 0; m < num_examples; m++)
		{
			for (n = 0; n < attribute_amount; n++)
			{

				int index = findAttribute(buf[m][n + 1], &attr);

				for (e = 0; e < outcome_type_amount; e++)
				{
					if (strcmp(outcome_string[e], buf[m][attribute_amount + 1]) == 0)
					{
						examples[n].value[index].outcome[e] = examples[n].value[index].outcome[e] + 1;
						training_Outcome_Proportion[e] = training_Outcome_Proportion[e] + 1;
					}
				}
			}
		}
		int delete_counter = 0;
		if (FIND_IRRELEVANT == 1)
		{
			for (n = 0; n < attribute_amount; n++)
			{
				if (isIrrelevantAttr(buf, num_examples, n, IrreAttr))
				{
					indexOfIrreAttr[amountOfIrreAttr] = n;
					IrreAttr[amountOfIrreAttr++] = deleteAttribute(examples, n);

					delete_counter++;
					continue;
				}
			}
		}
		attribute_amount = attribute_amount - delete_counter;
		if (FIND_CONTRADICTION == 1)
			cmpAndDelete(buf, num_examples);
		//every line is checked by "attribute_amount" attributes, so should divid attribute_amount
		int a;
		for (a = 0; a < outcome_type_amount; a++)
		{
			training_Outcome_Proportion[a] = training_Outcome_Proportion[a] / attribute_amount;
		}
	}
}



char * deleteAttribute(Attribute exp[], int index)
{
	int i;
	char *attr[20];
	strcpy(attr, exp[index].attr_name);
	for (i = index; i < attribute_amount; i++)
	{
		if (i == attribute_amount - 1)
		{
			//exp[i] = (Attribute)0;  // we can ignore the last repeated one.
			break;
		}
		exp[i] = exp[i + 1];
	}
	return attr;
}

void split(char *str)
{/*
 int i = 0;
 int j = 0;
 int head = 0;
 char temp;
 char *buf[20];
 while(str != '\0')
 {
 temp = str + head + i;

 if(temp = ' ')
 {
 if((temp - 1) == ' ')
 {
 i = 0;
 head++;
 continue;
 }
 memcpy(buf[j], str + head, i - 1);
 head = i + 1;
 i = -1;
 j++;
 }
 i++;
 }*/
}

int isRemoved(index)
{
	int i;
	for (i = 0; i < amountOfIrreAttr; i++)
	{
		if (index == indexOfIrreAttr[i])
			return 1;
	}
	return 0;
}

void cmpAndDelete(char *buf[200][200], int num_of_example)
{
	int store[50][50];
	int group = 0;
	int index;
	memset(store, -1, sizeof(store));
	findClassError(buf, num_of_example, store);
	for (group = 0; group < 50; group++)
	{
		if (store[group][1] == -1)
			continue;
		for (index = 0; index < 50; index++)
		{
			int row = store[group][index];
			if (row == -1)
				continue;
			int dd;
			float product_proba1 = 1;
			float product_proba2 = 1;
			int idx_a, idx_v;
			for (dd = 0; dd < attribute_amount; dd++)
			{
				findAttributeNValueIndex(buf[row][dd + 1], &idx_a, &idx_v);
				float out1 = (float)examples[idx_a].value[idx_v].outcome[0] - 1;
				float out2 = (float)examples[idx_a].value[idx_v].outcome[1] - 1;

				product_proba1 = product_proba1 * (out1 / (out1 + out2));
				product_proba2 = product_proba2 * (out2 / (out1 + out2));
			}
			idx_a = 0;
			idx_v = 0;
			if (product_proba1 > product_proba2)
			{
				if (strcmp(outcome_string[0], buf[row][attribute_amount + 1]) != 0)
				{
					for (dd = 0; dd < attribute_amount; dd++)
					{
						findAttributeNValueIndex(buf[row][dd + 1], &idx_a, &idx_v);
						examples[idx_a].value[idx_v].outcome[0] = examples[idx_a].value[idx_v].outcome[0];
						examples[idx_a].value[idx_v].outcome[1] = examples[idx_a].value[idx_v].outcome[1] - 1;
					}
				}
			}
			else if (product_proba1 < product_proba2)
			{
				if (strcmp(outcome_string[1], buf[row][attribute_amount + 1]) != 0)
				{
					for (dd = 0; dd < attribute_amount; dd++)
					{
						findAttributeNValueIndex(buf[row][dd + 1], &idx_a, &idx_v);
						examples[idx_a].value[idx_v].outcome[1] = examples[idx_a].value[idx_v].outcome[1];
						examples[idx_a].value[idx_v].outcome[0] = examples[idx_a].value[idx_v].outcome[0] - 1;
					}
				}
			}

		}
	}

}
// find the error in the training data,
// for example, there are two rows haing the same values but different classification
void findClassError(char *buf[200][200], int num_of_example, int store[50][50]) // the third param is output
{
	// used for checking if there is error, ==0 means classification errors exist
	int n, m, i = 0;
	//	int store[50][50]; //the first index means the number of groups that may have error, the second means the specific rows per group.
	int temp[100];
	int count = -1;
	int row = 0;
	int count_for_temp = 0;
	int flag = 0;
	for (m = 0; m < num_of_example; m++)
	{
		int cc;
		for (cc = 0; cc < count_for_temp; cc++)
		{
			if (temp[cc] == m)
				flag = 1;
		}
		if (flag == 1)
			continue;
		count++;
		row = 0;
		store[count][row++] = m;
		temp[count_for_temp++] = m;
		for (n = m + 1; n < num_of_example; n++)
		{
			for (i = 0; i < attribute_amount; i++)
			{

				if (strcmp(buf[m][i + 1], buf[n][i + 1]) != 0)
					break;
			}
			if (i >= (attribute_amount))
			{
				//it means same value of conditional attributes but different classification
				if (strcmp(buf[m][attribute_amount + 1], buf[n][attribute_amount + 1]) != 0)
					//if(buf[m][attribute_amount + 1] !=  buf[n][attribute_amount + 1])
				{
					store[count][row++] = n;
					temp[count_for_temp++] = n;

					return 0; // it's a relevant attribute.
				}

			}

		}
	}
}

// if there has been one or more than one irrelevant attr, checking if the new attri is irr require ignore the old irrelevant attr in buf[][]
int isIrrelevantAttr(char *buf[200][200], int num_of_example, int remove_index)
{
	int n, m, i = 0;
	//int attr[outcome_type_amount][100];

	for (m = 0; m < num_of_example; m++)
	{
		for (n = m + 1; n < num_of_example; n++)
		{
			for (i = 0; i < attribute_amount; i++)
			{
				if (i == remove_index || isRemoved(i)) // attribute with this index could not be compared.
					continue;
				if (strcmp(buf[m][i + 1], buf[n][i + 1]) != 0)
					break;
				//if(buf[m][i] != buf[n][i])

			}
			if (i >= (attribute_amount))
			{
				//it means same value of conditional attributes but different classification
				if (strcmp(buf[m][attribute_amount + 1], buf[n][attribute_amount + 1]) != 0)
					//if(buf[m][attribute_amount + 1] !=  buf[n][attribute_amount + 1])
				{
					return 0; // it's a relevant attribute.
				}

			}

		}

		//}			
	}
	return 1;

}

// return the total number of the attribute
// this method requires no space at the end of the last character, no more than one spaces between two words
int readFile(char *path, char *buf[200][200], FILE_TYPE file_type)
{
	char temp[2];
	//char *buf[200][200];
	int n = 0;
	int m = 0;

	FILE *file = fopen(path, "r");

	if (file == NULL)
		return -1;
	else
	{
		while (!feof(file))
		{
			fgets(temp, 2, file);
			if (temp[0] == ' ')
			{
				m++;
				continue;
			}
			else if (temp[0] == '\n')
			{
				char  a[2];
				if (m != 0)
				{
					_itoa(m + 1, a, 10); // indicate the total number of words in every line
					buf[n][0] = (char *)malloc(2 * sizeof(char));
					memcpy(buf[n][0], a, 2);
					//buf[n][0] = a;
					n++;
				}

				m = 0;
				continue;
			}
			else
			{
				if (buf[n][m + 1] == '\0')
					buf[n][m + 1] = "";

				buf[n][m + 1] = str_combine(buf[n][m + 1], temp);
				//strcat(buf[n][m + 1],&temp);
			}
		}
	}
	if (file_type == FILE_ATTRIBUTE)
		return n - 1; // n is the total lines of the file, the total amount of the attribute must exclude the last two lines, one is empty line ,another is the line of 'outcome'
	else if (file_type == FILE_TRAINING)
	{
		return n;
	}
	else
		return -1;

	fclose(file);
}

// if type == ALL_ATTRIBUTES, attribute will be ignored, so pass null to it.
float calEntropy(Attribute attribute, char *val_name, EXAMPLE_TYPE type)
{

	Value val;
	float entropy = 0.0;
	int i = 0;

	int out_type_num = 0;
	int examplesOfAttr = 0;

	if (type == ALL_ATTRIBUTES_TYPE)   // cal entropy for the whole training data
	{
		int a;
		for (a = 0; a < outcome_type_amount; a++)
		{
			float q = (float)training_Outcome_Proportion[a] / (float)num_examples;
			entropy = entropy + (-1) * q * (log10f(q) / log10(2));
		}
		return entropy;
	}

	if (type == VALUE_TYPE)
	{
		int j = 0;
		while (strcmp(attribute.value[j].val_name, val_name) != 0)
		{
			j++;
		}

		val = attribute.value[j];
		out_type_num = val.outcome_type_num;
		int n;
		for (n = 0; n < out_type_num; n++)
		{
			examplesOfAttr = examplesOfAttr + val.outcome[n];
		}
	}

	else if (type == ATTRI_TYPE)
	{
		int m = 0;
		int n = 0;

		for (m = 0; m < attribute.value_type_num; m++)
		{
			out_type_num = attribute.value[m].outcome_type_num;
			val = attribute.value[m];

			for (n = 0; n < out_type_num; n++)
			{
				examplesOfAttr = examplesOfAttr + val.outcome[n];
			}

		}
	}
	int n;
	for (n = 0; n < out_type_num; n++)
	{
		float proportion = (float)val.outcome[n] / (float)examplesOfAttr;
		// if proportion = 0, then log10f(0) is a large number, but 0 times any number is 0;
		// if proportion = 1, it means all the class is the same. So entropy = 0;
		if (proportion == 0)
		{
			entropy = 0;
			continue;
		}
		if (proportion == 1)
			return 0;
		entropy = entropy + (-1) * proportion * (log10f(proportion) / log10(2));
	}
	return entropy;


}

//e.g. cal how many sunny in the Attr Sky.
int calValueAmount_name(char *val_name)
{
	Attribute attr;
	int index = findAttribute(val_name, &attr);
	int j;
	int count = 0;

	for (j = 0; j <attr.value[index].outcome_type_num; j++)
	{
		count = count + attr.value[index].outcome[j];
	}
	return count;
}

char *cmpOutcome(char *val_name)
{
	Attribute attr;
	int index = findAttribute(val_name, &attr);
	int j;
	int idx = 0;
	int max = attr.value[index].outcome[0];
	for (j = 1; j <attr.value[index].outcome_type_num; j++)
	{
		if (max < attr.value[index].outcome[j])
		{
			max = attr.value[index].outcome[j];
			idx = j;
		}
	}
	return outcome_string[idx];
}

int calValueAmount_index(Attribute attr, int val_index)
{
	int j;
	int count = 0;
	for (j = 0; j <attr.value[val_index].outcome_type_num; j++)
	{
		count = count + attr.value[val_index].outcome[j];
	}
	return count;
}

void findAttributeNValueIndex(char *val_name, int *attr_idx, int *v_idx)
{
	int i, j;
	int index = -1;
	for (i = 0; i < attribute_amount; i++)
	{
		for (j = 0; j < examples[i].value_type_num; j++)
		{

			if (strcmp(examples[i].value[j].val_name, val_name) == 0)
			{
				*attr_idx = i;
				*v_idx = j;
				break;
			}
		}
	}

}

//return the index of Value
int findAttribute(char *val_name, Attribute *attr)
{
	int i, j;
	int index = -1;
	for (i = 0; i < attribute_amount; i++)
	{
		for (j = 0; j < examples[i].value_type_num; j++)
		{

			if (strcmp(examples[i].value[j].val_name, val_name) == 0)
			{
				*attr = examples[i];
				index = j;
				return index;
			}
		}
	}
	return index;

}

int calExampleAmount(int index)
{/*
 int i,j;
 while(examples[i].attr_name != attr_name)
 {
 i++;
 }

 // need to determine whether has the attr_name in examples;
 int m;
 int value_out_num; // total amount of the value, e.g. how many value "sunny" in the attr "Sky"?
 for(j = 0; j < examples[i].value_type_num; j++)
 {
 for(m = 0; m < examples[i].value[j].outcome_type_num; m++)
 {
 value_out_num = value_out_num + examples[i].value[j].outcome[m];
 }
 s_value = s_value + calEntropy(examples[i].attr_name,  examples[i].value[j].val_name, ALL_ATTRIBUTES_TYPE);
 }*/
}

// example_name may be some certain value, or it represents the whole training data.
// if it is a value name, find out which attribute this value belong to. because the value information is stored in the struct 'Attribute'.
float calInfoGain(char *example_name, char *estimated_attr_name, EXAMPLE_TYPE type)
{
	float s_example = 0.0;
	char * attr_name;
	float s_value = 0.0;
	int exmpl_num = 0;
	Attribute attr;
	//calculate the entropy of example, that is S in Gain(S,A).
	//---START---
	if (type == VALUE_TYPE)
	{
		findAttribute(example_name, &attr);
		s_example = calEntropy(attr, example_name, VALUE_TYPE);
		exmpl_num = calValueAmount_name(example_name);

	}
	else if (type == ALL_ATTRIBUTES_TYPE)
	{
		//Attribute attr_null;
		s_example = calEntropy(attr, "", ALL_ATTRIBUTES_TYPE);
		//get the amount of training data
		exmpl_num = num_examples;
	}
	//--END---

	//calculate the entropy of every value in A [Gain(S,A)].
	//---START---
	int i = 0;
	while (strcmp(examples[i].attr_name, estimated_attr_name) != 0)
	{
		i++;
	}

	// need to determine whether has the attr_name in examples;
	int m;
	int value_out_num; // total amount of the value, e.g. how many value "sunny" in the attr "Sky"?
					   /*
					   for(j = 0; j < examples[i].value_type_num; j++)
					   {
					   for(m = 0; m < examples[i].value[j].outcome_type_num; m++)
					   {
					   value_out_num = value_out_num + examples[i].value[j].outcome[m];
					   }
					   s_value = s_value + calEntropy(examples[i].attr_name,  examples[i].value[j].val_name, ALL_ATTRIBUTES_TYPE);
					   }*/
	int val_amount;
	for (m = 0; m < examples[i].value_type_num; m++)
	{
		val_amount = calValueAmount_index(examples[i], m);
		float temp = (float)val_amount / (float)exmpl_num;
		s_value = s_value + temp * calEntropy(examples[i], examples[i].value[m].val_name, VALUE_TYPE);
	}

	return s_example - s_value;
}

//most common value is that outcome when the the outcome of examples is the same
OUTCOME_STATE searchExmpOutState(char *example_name, char *most_common_output)
{
	Attribute attr;
	int i;

	if (example_name == "")
	{
		return DIFF_OUTCOME;
	}

	int index = findAttribute(example_name, &attr);
	Value val = attr.value[index];
	int max = val.outcome[0];
	int sum = max;
	int outcome_index = 0;
	for (i = 1; i < val.outcome_type_num; i++)
	{
		sum = sum + val.outcome[i];
		if (max < val.outcome[i])
		{
			max = val.outcome[i];
			outcome_index++;
		}
	}
	memcpy(most_common_output, outcome_string[outcome_index], strlen(outcome_string));
	if (max == sum) // it means all exmaples are the same outcome
	{
		return SAME_OUTCOME;
	}
	return DIFF_OUTCOME;
}

BOOL checkAttrAvailable(int attr_remove[], int index)
{
	int j = 0;
	for (j = 1; j < attribute_amount + 1; j++)
	{
		if (attr_remove[j] == index)
		{
			return FALSE;
		}
	}
	return TRUE;
}

// attr_index is 0 at the first time invoking this function,
//and attr_index + 1 when do recursion once, untill reach to the max num of attr
int count = 0;
void ID3(char *example_name, DecisionTree *root, EXAMPLE_TYPE type, int *attr_remove_idx, int attr_remove)
{
	count++;
	//DecisionTree root;
	int attr_remove_temp[150] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	memset(attr_remove_temp, -1, sizeof(attr_remove_temp));
	int attr_remove_count = attr_remove;
	// attr_remove_temp[attr_remove_count] = attr_remove_idx;
	memcpy(attr_remove_temp, attr_remove_idx, (attr_remove + 1) * sizeof(attr_remove_idx));

	char most_common[10];
	/* if(count == 0)
	{
	for(i = 0; i < 10; i++)
	{
	attr_remove_index[i] = -1;
	}

	}*/
	//when the outcome of exmples are the same, then the 'root' will be the leaf node
	if (searchExmpOutState(example_name, most_common) == SAME_OUTCOME)
	{
		strcpy(root->node_name, most_common);
		//root->node_name = most_common;
		root->branch_num = 0;
		return;
	}


	//give the info gain of first available attribute to the maxGain
	float maxGain = 0.0;
	int j;
	for (j = 0; j < attribute_amount; j++)
	{
		if (!checkAttrAvailable(attr_remove_temp, j))
			continue;
		maxGain = calInfoGain(example_name, examples[j].attr_name, type);
		break;
	}

	if (j == attribute_amount)  //attributes are all considered.
	{
		strcpy(root->node_name, most_common);
		root->branch_num = 0;
		return;
	}

	int max_attr_index = j;
	float temp;
	j++;
	for (; j < attribute_amount; j++)
	{
		if (!checkAttrAvailable(attr_remove_temp, j))
			continue;
		if (maxGain < (temp = calInfoGain(example_name, examples[j].attr_name, type)))
		{
			maxGain = temp;
			max_attr_index = j;     // find the attribute with the highest info gain
		}
	}
	strcpy(root->node_name, examples[max_attr_index].attr_name);
	if (count == 1)
	{
		int a = 0;
		for (a = 0; a < amountOfIrreAttr; a++)
		{
			strcat(root->node_name, " ^ ");
			strcat(root->node_name, IrreAttr[a]);
		}


	}
	attr_remove_temp[attr_remove_count + 1] = max_attr_index;
	attr_remove_count++;
	attr_remove_temp[0] = attr_remove_count;
	int i;
	for (i = 0; i < examples[max_attr_index].value_type_num; i++)
	{
		root->branches_value[i] = examples[max_attr_index].value[i].val_name;
		root->branch_num = examples[max_attr_index].value_type_num;
		root->branches[i] = (DecisionTree *)malloc(sizeof(DecisionTree));
		// memcpy(attr_remove_index,attr_remove_temp,sizeof(attr_remove_temp));

		ID3(root->branches_value[i], root->branches[i], VALUE_TYPE, attr_remove_temp, attr_remove_count);
	}
}

void printDesicionTree(DecisionTree *root, int blk)
{
	int i, j;
	for (i = 0; i<blk; i++)
		printf("    ");
	printf("|   %s", root->node_name);
	if (strcmp(root->node_name, "Yes") == 0 || strcmp(root->node_name, "No") == 0)
	{
		printf("\n");
	}

	for (j = 0; j < root->branch_num; j++)
	{

		printf("= %s \n", root->branches_value[j]);
		printDesicionTree(root->branches[j], blk + 1);
		if (j < root->branch_num - 1)
		{
			for (i = 0; i<blk; i++)
				printf("    ");
			printf("|   %s", root->node_name);
		}
	}
}


void printDesicionTreeByLine(DecisionTree *root, char _name[10][50], int print_count)
{
	char name[10][50];
	int count11 = 0;
	count11 = print_count;
	int aa = 0;
	for (aa = 0; aa < count11; aa++)
	{
		strcpy(name[aa], _name[aa]);
	}

	strcat(name[count11], root->node_name);
	count11 = print_count;
	if (root->branch_num == 0)
	{
		int a;
		for (a = 0; a < count11; a++)
		{
			printf(name[a]);
		}
		printf("\n");
		return;
	}
	int j;
	for (j = 0; j < root->branch_num; j++)
	{
		strcat(name[count11], "=");
		if (count11 == 0)
		{
			int aaaa = 0;
		}
		strcat(name[count11], root->branches_value[j]);
		count11 = print_count;
		count11 = count11 + 1;
		printDesicionTreeByLine(root->branches[j], name, count11);
	}
}
/*
void printDesicionTree(DecisionTree *tree)
{
int i;
char name[60] = {'\0'};
char temp[3] = "|  ";
strcat(name,tree->node_name);
if(tree->branch_num == 0)
{
printf(" %s\n",name);
return;
}
strcat(name, " = ");
for(i = 0; i < tree->branch_num; i++)
{
strcat(name, tree->branches_value[i]);
strcat(name, "\n");
printf("%s", name);
printDesicionTree(tree->branches[i]);
}

}*/


int rule_arr_num = 0;
void createRules(DecisionTree *tree, Rule *preRule, Rule *rule)
{


	rule->prevRule = preRule;
	strcpy(rule->name, tree->node_name);

	if (tree->branch_num == 0)
	{
		rule_array[rule_arr_num] = rule;
		rule_arr_num++;
	}

	int i;
	for (i = 0; i < tree->branch_num; i++)
	{
		strcpy(rule->value, tree->branches_value[i]);
		rule->nextRule = (Rule *)malloc(sizeof(Rule));
		createRules(tree->branches[i], rule, rule->nextRule);
	}
}

void postPruning(char *buf[200][200])
{
	int i;
	Rule *r = NULL;
	Rule temp;
	for (i = 0; i <rule_arr_num; i++)
	{
		r = rule_array[i]->prevRule;
		r = r->prevRule;
		strcpy(temp.name, cmpOutcome(r->value));
		memcpy(r->nextRule, &temp, sizeof(temp));
	}
	calAccuracy_rule(buf, r);
}

int rulecount = 0;
void printRule(Rule *r[])
{
	int i;
	//Rule *r = rule_array[0];
	for (i = 0; i <rule_arr_num; i++)
	{
		if (rulecount == 0)
		{
			float total = r[i]->true_false_num[0] + r[i]->true_false_num[1];
			float true_percent = (float)r[i]->true_false_num[0] / (float)total;
			float false_percent = (float)r[i]->true_false_num[1] / (float)total;
			printf("- (T_%: %.2f  ,  F_%: %.2f)", true_percent, false_percent);
		}
		rulecount++;

		while (r[i])
		{
			printRule(r[i]->prevRule);
			printf("- %s", r[i]->name);
		}
	}
}

void changeLine(char *buf[200][200], int row_des, int row_source, int col_num)
{
	int i = 0;
	char temp[20] = { '\0' };
	for (i = 0; i < col_num; i++)
	{
		strcpy(temp, buf[row_source][i]);
		strcpy(buf[row_source][i], buf[row_des][i]);
		strcpy(buf[row_des][i], temp);
	}
}
// numbers of attribute in 'examples' will be changed according to the threshold
void genContiAttr(char *buf[200][200])
{
	int i, j;
	char temp[50] = { '\0' };
	int min;
	int m = -1;
	int attr_threshold_amount = 0;
	int attr_count = 0;
	char attr_name[10][15];
	for (i = 0; i < attribute_amount; i++) {
		strcpy(attr_name[i], examples[i].attr_name);
	}

	for (i = 0; i < attribute_amount; i++)
	{
		for (j = 0; j < num_examples; j++)
		{
			for (m = j; m < num_examples; m++)
			{
				if (atof(buf[j][i + 1]) > atof(buf[m][i + 1]))
				{
					/* temp = buf[j][i + 1];
					buf[j][i + 1] = buf[m][i + 1];
					buf[m][i + 1] = temp;*/

					changeLine(buf, j, m, 6);
				}
			}
		}
		m = 0;
		char *outcome_temp = buf[0][attribute_amount + 1];
		int outcome_index = attribute_amount + 1;
		float threshold[100]; //every attribute have some threshold


		m = 0;
		for (j = 1; j < num_examples; j++)
		{
			if (strcmp(buf[j][outcome_index], outcome_temp) != 0)
			{
				threshold[m] = (atof(buf[j][i + 1]) + atof(buf[j - 1][i + 1])) / 2;
				outcome_temp = buf[j][attribute_amount + 1];
				m++;
			}
		}
		attr_threshold_amount = m;// it means amount, not index

		calContiValOutcome(threshold, attr_threshold_amount, buf, i, attr_name);
		attr_count = attr_count + attr_threshold_amount;
	}
	attribute_amount = attr_count;

}
int num = 0;
void calContiValOutcome(float threshold[100], int amount, char *buf[200][200], int attr_index, char attr_name[15][10])
{
	//int m = 0;
	char string[5] = { '\0' };
	int i, j;

	for (j = 0; j < amount; j++)
	{
		if (num >= 4)
		{
			memcpy(examples[num].attr_name, attr_name[attr_index], strlen(attr_name[attr_index]));
		}
		strcat(examples[num].attr_name, " > ");
		strcat(examples[num].attr_name, _gcvt(threshold[j], 2, string));
		examples[num].attr_type = CONTINUOUS_VAL_ATTR;
		examples[num].value_type_num = 2;
		strcpy(examples[num].value[0].val_name, "T");
		strcpy(examples[num].value[1].val_name, "F");
		num++;
	}
	int n = 0;
	int index = -1;
	int a = 0;
	int k;
	for (k = 0; k < amount; k++)
	{
		for (j = 0; j < num_examples; j++)
		{
			if (atof(buf[j][attr_index + 1]) < threshold[k])  // belong to 'false'
			{
				index = findOutcomeIndex(buf[j][attribute_amount + 1]);
				examples[k].value[1].outcome[index] = examples[k].value[1].outcome[index] + 1;
			}
			else  //belong to 'true'
			{
				index = findOutcomeIndex(buf[attr_index][attribute_amount + 1]);
				examples[k].value[0].outcome[index] = examples[k].value[0].outcome[index] + 1;
			}
			a = a + 1;
		}

	}
	//attribute_amount = attribute_amount + m;
}

int findOutcomeIndex(char *name)
{
	int i;
	for (i = 0; i < outcome_type_amount; i++)
	{
		if (strcmp(outcome_string[i], name) == 0)
		{
			return i;
		}
	}
	return -1;
}

void f(int a, int b)
{
	int c;
	c = 8;
	printf("%x\n", &c);
	while (b > 0)
	{
		f(a, --b);
		f(a, --b);
	}
}

void calAccuracy_rule(char *buf[200][200], Rule *r)
{
	char leafname[20];
	Rule *leafRule = r;
	Rule *temp;
	int isExist = 1;
	strcpy(leafname, r->name);
	int i, j;
	for (i = 0; i < num_examples; i++)
	{
		r = leafRule;
		while (r->prevRule)
		{
			for (j = 0; j < attribute_amount; j++)
			{
				temp = (r->prevRule);
				if (strcmp(temp->value, buf[i][j + 1]) == 0)
				{
					break;
				}
			}
			if (j == attribute_amount)
			{
				isExist = 0;
				break;
			}
		}
		if (isExist == 0)
			continue;
		if (strcmp(leafname, buf[i][attribute_amount + 1]) == 0) //equal
		{
			r->true_false_num[0] = r->true_false_num[0] + 1; // root and leaf
			leafRule->true_false_num[0] = leafRule->true_false_num[0] + 1;
		}
		else
		{
			r->true_false_num[1] = r->true_false_num[1] + 1;
			leafRule->true_false_num[1] = leafRule->true_false_num[1] + 1;
		}
	}
}

void calAccuracy_tree(char *buf[200][200], DecisionTree *root)
{
	int i, j, m;
	Attribute attr;

	for (i = 0; i < num_examples; i++)
	{

		for (m = 0; m < root->branch_num; m++)
		{
			for (j = 0; j < attribute_amount; j++)
			{
				if (strcmp(root->branches_value[m], buf[i][j + 1]) == 0) //equal
				{
					root = root->branches[m];
					m = 0;
				}
			}
		}
		if (root->branch_num == 0)
		{
			if (strcmp(root->node_name, buf[i][attribute_amount + 1]) == 0)
			{
				root->true_false_num[0] = root->true_false_num[0] + 1;
			}
			else
				root->true_false_num[1] = root->true_false_num[1] + 1;
		}
	}
}

int main(int argc, char *argv[])
{
	DecisionTree root;
	char *buf[200][200];
	printf("Please choose one kind of decision tree to create. Y - discrete, N - continuous");
	char a;

	scanf("%c", &a);

	if (a == 'Y')
	{
		char *buf_attr[100][100];
		attribute_amount = readFile("C:\\Users\\John_\\Documents\\DecisionTreeImp\\tennis-attr.txt", buf_attr, FILE_ATTRIBUTE);

		initExample(buf_attr, FILE_ATTRIBUTE);
		//memset(buf_att,-1,sizeof(buf));

		num_examples = readFile("C:\\Users\\John_\\Documents\\DecisionTreeImp\\train2.txt", buf, FILE_TRAINING);


		initExample(buf, FILE_TRAINING);

		int temp[10] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };;
		ID3("", &root, ALL_ATTRIBUTES_TYPE, temp, 0);
		printDesicionTree(&root, 0);


	}

	return 0;
}

