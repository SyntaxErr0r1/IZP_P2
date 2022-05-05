/**
 * Project:    Projekt 2 - Práce s datovými strukturami
 * Name:       Juraj Dedic
 * Login:      xdedic07
 * File:       sps.c
 * Date:       6.12.2020
 * */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#define MAX 1000

#define INIT_CMD(cmd) create_cmd(cmds.list, &cmds.size, #cmd, cmd##_action)

typedef struct cell_t {
    int size;
    int cap;
    char *data;
} Cell;
typedef struct row_t {
    int size;
    int cap;
    Cell *cells;
} Row;
typedef struct table_t {
    int size;
    int cap;
    Row *rows;
} Table;
typedef struct selection_t{
    int row_min;
    int cell_min;

    int row_max;
    int cell_max;
} Selection;

typedef struct cmd_t {
    int (* action)(); 
    char * flag;
    char * param;
    //char selector[MAX];
} Cmd;

typedef struct cmd_list_t {
    Cmd list[MAX];
    int size;
} Cmd_l; 
typedef struct vars_t {
    char *list[10];
    char saved_selection[1000];
    char last_selection[1000];
} Vars; 
 
/**
 * CMD STRUCT COMMANDS
 */
void create_cmd(Cmd * cmd_list,int * cmd_list_size , char * cmd_flag, int (*cmd_function)()){
    //Selector s = {0,0,0,0};
    Cmd new_cmd = {
        .flag = cmd_flag,
        .param = NULL,   
        .action = cmd_function
    };
    cmd_list[*cmd_list_size] = new_cmd;
    (*cmd_list_size)++;
}
int cmds_append(Cmd_l * array, Cmd cmd){
    array->list[array->size] = cmd;
    return array->size++;
}

/**
 * *CELL FUNCTIONS
 */
void cell_ctor(Cell *a)
{
    a->size = 0;
    a->cap = 0;
    a->data = NULL;
}
 
void cell_dtor(Cell *a)
{
    if (a->cap)
        free(a->data);
    a->data = NULL;
    a->cap = a->size = 0;
}
 
void cell_resize(Cell *a, int new_cap)
{
    char *resized;
    resized = realloc(a->data, new_cap * sizeof(char));
    if (resized != NULL)
    {
        a->data = resized;
        a->cap = new_cap;
    }
}
 
void cell_append(Cell *a, char item)
{
    if (a->size == a->cap)
        cell_resize(a, a->cap ? a->cap + a->cap : 1);
 
    if (a->size < a->cap)
    {
        a->data[a->size] = item;
        a->size ++;
    }
}
 
void cell_insert(Cell *a, char item, int idx)
{
    if (a->size == a->cap)
        cell_resize(a, a->cap ? a->cap * 2 : 1);
 
    if (a->size < a->cap)
    {
        for (int i = a->size - 1; i >= idx; i--)
            a->data[i+1] = a->data[i];
 
        a->data[idx] = item;
        a->size ++;
    }
}
void cell_set(Cell *a, char * str)
{
    cell_dtor(a);
    int size = strlen(str);
    for (int i = 0; i < size; i++)
    {
        cell_append(a, str[i]);
    }
}

/**
 * *ROW FUNCTIONS
 */
void row_ctor(Row *a)
{
    a->size = 0;
    a->cap = 0;
    a->cells = NULL;
}
 
void row_dtor(Row *a)
{
    if (a->cap){
        free(a->cells);
    }
    a->cells = NULL;
    a->cap = a->size = 0;
}
void row_dtor_deep(Row *a)
{
    if (a->cap){
        for (int i = 0; i < a->size; i++)
        {
            cell_dtor(&a->cells[i]);
        }
        free(a->cells);
    }
    a->cells = NULL;
    a->cap = a->size = 0;
}
 
void row_resize(Row *a, int new_cap)
{
    Cell *resized;
    resized = realloc(a->cells, new_cap * sizeof(Cell));
    if (resized != NULL)
    {
        a->cells = resized;
        a->cap = new_cap;
    }
}
 
void row_append(Row *a, Cell item)
{
    if (a->size == a->cap)
        row_resize(a, a->cap ? a->cap + a->cap : 1);
 
    if (a->size < a->cap)
    {
        a->cells[a->size] = item;
        a->size ++;
    }
}
 
void row_insert(Row *a, Cell item, int idx)
{
    if (a->size == a->cap)
        row_resize(a, a->cap ? a->cap * 2 : 1);
 
    if (a->size < a->cap)
    {
        for (int i = a->size - 1; i >= idx; i--)
            a->cells[i+1] = a->cells[i];
 
        a->cells[idx] = item;
        a->size ++;
    }
}
void row_remove_dt(Row *a, int col_idx){
    cell_dtor(&a->cells[col_idx]);
    for (int i = col_idx; i <= a->size-1; i++){
        a->cells[i] = a->cells[i+1];
        //printf("Moving cell %d up\n",i+1);
    }
    a->size --;
}
void row_remove(Row *a, int col_idx){
    for (int i = col_idx; i <= a->size-1; i++){
        a->cells[i] = a->cells[i+1];
        //printf("Moving cell %d up\n",i+1);
    }
    a->size --;
}
void row_fill(Row *a, int n){
    for (int i = 0; i < n; i++)
    {
        Cell empty_cell;
        cell_ctor(&empty_cell);
        row_append(a, empty_cell);
    }
}

/**
 * *TABLE FUNCTIONS
 */
void table_ctor(Table *a)
{
    a->size = 0;
    a->cap = 0;
    a->rows = NULL;
}
 
void table_dtor(Table *a)
{
    if (a->cap){
        free(a->rows);
    }
    a->rows = NULL;
    a->cap = a->size = 0;
}
 
void table_resize(Table *a, int new_cap)
{
    Row *resized;
    resized = realloc(a->rows, new_cap * sizeof(Row));
    if (resized != NULL)
    {
        a->rows = resized;
        a->cap = new_cap;
    }
}
 
void table_append(Table *a, Row item)
{
    if (a->size == a->cap)
        table_resize(a, a->cap ? a->cap + a->cap : 1);
 
    if (a->size < a->cap)
    {
        a->rows[a->size] = item;
        a->size ++;
    }
}
 
void table_insert(Table *a, Row item, int idx)
{
    if (a->size == a->cap)
        table_resize(a, a->cap ? a->cap * 2 : 1);
 
    if (a->size < a->cap)
    {
        for (int i = a->size - 1; i >= idx; i--)
            a->rows[i+1] = a->rows[i];
 
        a->rows[idx] = item;
        a->size ++;
    }
}

void table_remove(Table *a, int row_idx){
    row_dtor_deep(&a->rows[row_idx]);
    for (int i = row_idx; i <= a->size-1; i++){
        a->rows[i] = a->rows[i+1];
        //printf("Moving row %d up\n",i+1);
    }
    a->size --;
}

void table_print(Table t){
    printf("Printing table (size: %d, cap: %d)\n",t.size,t.cap);
    for (int i = 0; i < t.size; i++)
    {
        Row r = t.rows[i];
        putc('|', stdout);
        for (int j = 0; j < r.size; j++)
        {
            Cell c = r.cells[j];
            putc(' ',stdout);
            for (int i = 0; i < 8; i++)
            {
                int ch = (i < c.size) ? c.data[i] : ' ';
                putc(ch, stdout);
            }
            fprintf(stdout, " |");
        }
        putc('\n',stdout);
    }
}
bool is_delim(char c, char *delim){
    int delim_len = strlen(delim);
    for (int i = 0; i < delim_len;i++)
    {
        if(delim[i] == c)
            return true;
    }
    return false;
}
void table_load(Table *table, FILE *file, char * delim){
    char c = 0;
    while (c != EOF)
    {
        c = fgetc(file);
        Row row;
        row_ctor(&row);
        while (c!='\n' && c != EOF)
        {
            Cell cell;
            cell_ctor(&cell);
            while (c != EOF && !is_delim(c, delim) && c!='\n')
            {
                if(c != '\r'){
                    cell_append(&cell, c);
                }
                c = fgetc(file);
                //printf("Nest 3");
            }
            row_append(&row, cell);
            //cell_print(cell);
            if(c!= '\n')
                c = fgetc(file);
            //cell_dtor(&cell);
            //printf("Nest 2 c:%c",c);
        }
        table_append(table, row);
        //row_dtor(&row);
        //printf("Nest 1");
    }
}

void table_align(Table *t){
    int max_row_size = 0;
    for (int i = 0; i < t->size; i++)
    {
        if(t->rows[i].size > max_row_size)
            max_row_size = t->rows[i].size;
    }
    //printf("Found max row: %d\n",max_row_size);
    for (int i = 0; i < t->size; i++)
    {
        int reps = max_row_size - t->rows[i].size;
        for (int j = 0; j < reps; j++)
        {
            Cell empty_cell;
            cell_ctor(&empty_cell);
            row_append(&t->rows[i], empty_cell);
        }
    }
}

void usage_print(FILE *const stream){
    fprintf(stream,"ERR> Usage is: ./sps [-d DELIM] CMD_SEQUENCE FILE\n");
}

void sequence_separate(char **sequence_arr, int * sequence_size, char *sequence_str, char *delim){
    char * token = strtok(sequence_str, delim);
    while( token != NULL ) {
        //printf("Writing %s at index %d\n", token, *sequence_size);
        sequence_arr[*sequence_size] = token;
        token = strtok(NULL, delim);
        (*sequence_size)++;
    }
}

bool starts_with(const char *a, const char *b)
{
   if(strncmp(a, b, strlen(b)) == 0) return 1;
   return 0;
}
void strip_flag_name (char *string, char *sub) {
	char *match;
	int len = strlen(sub);
    match = strstr(string, sub);
    *match = '\0';
    strcat(string, match+len+1);
}
/**
 * *sequence_queue() Iterates through the cmd sequence, appends commands into the queue
 * @param cmds      Stores all available commands and their functions
 * @param cmd_queue The actual queue where all found commands will be stored for execution
 * @param cmd_seq   Array of commands from the COMMAND SEQUENCE, where each command is in String format
 * @param cmd_seq_s Size of the command sequence array
 */
void sequence_queue(Cmd_l * cmds, Cmd_l * cmd_queue, char **cmd_seq, int cmd_seq_s){
    for (int i = 0; i < cmd_seq_s; i++)
    {
        if(cmd_seq[i][0] == '['){
            int pos = cmds_append(cmd_queue, cmds->list[0]);
            cmd_queue->list[pos].param = cmd_seq[i];
        }else{
            for (int j = 1; j < cmds->size; j++) //STARTING FROM 1 (selector cmd is at 0)
            {
                if(starts_with(cmd_seq[i], cmds->list[j].flag)){
                    //char param[MAX];
                    int len_flag = strlen(cmds->list[j].flag);
                    int len_cmd = strlen(cmd_seq[i]);
                    int pos = cmds_append(cmd_queue, cmds->list[j]);
                    //copy the param from the sequence, skip the flag name
                    if(len_cmd > len_flag+1){
                        //strncpy(cmd_queue->list[pos].param, cmd_seq[i] + (len_flag+2), len_cmd - (len_flag+2));  
                        //cmd_queue->list[pos].param = param;
                        cmd_queue->list[pos].param = cmd_seq[i];
                        strip_flag_name(cmd_queue->list[pos].param, cmds->list[j].flag);
                    }
                    //printf("Appending %s to the queue, with param %s!\n",cmd_queue->list[pos].flag, param);
                    //memset(param,0,strlen(param));
                }
            }
        }
    }
}
void reverse(char* str, int len) 
{ 
    int i = 0, j = len - 1, temp; 
    while (i < j) { 
        temp = str[i]; 
        str[i] = str[j]; 
        str[j] = temp; 
        i++; 
        j--; 
    } 
} 
  
// Converts a given integer x to string str[].  
// d is the number of digits required in the output.  
// If d is more than the number of digits in x,  
// then 0s are added at the beginning. 
int intToStr(int x, char str[], int d) 
{ 
    int i = 0; 
    while (x) { 
        str[i++] = (x % 10) + '0'; 
        x = x / 10; 
    } 
  
    // If number of digits required is more, then 
    // add 0s at the beginning 
    while (i < d) 
        str[i++] = '0'; 
  
    reverse(str, i); 
    str[i] = '\0'; 
    return i; 
} 


void cfrom(char *dest, char *src, int i_from, int i_to){
    int i_dest = 0;
    for(int i = i_from; i <= i_to; i++){
        dest[i_dest] = src[i];
        i_dest++;
    }
}
void posfrompar(char * param_raw, int * row, int * cell){
    char param[1000];
    cfrom(param, param_raw, 1, strlen(param_raw)-2);
    char *positions[2];
    int no_pos = 0;
    sequence_separate(positions, &no_pos, param, ",");
    *row = atoi(positions[0])-1;
    *cell = atoi(positions[1])-1;
}

/**
 * *Commands
*/
int selector_whole(Table * table, Selection *s){
    s->cell_min = 0;
    s->cell_max = table->rows[0].size-1;
    s->row_min = 0;
    s->row_max = table->size-1;
    return 0;
}
int selector_col(Table * table, Selection *s, char **positions){
    int col_num = atoi(positions[1])-1;
    if(col_num < 0){
        fprintf(stderr, "ERR> Column number must be greater than 0!\n");
        return 1;
    }
    s->cell_min = col_num;
    s->cell_max = col_num;
    s->row_min = 0;
    s->row_max = table->size-1;
    return 0;
}
int selector_row(Table * table, Selection *s, char **positions){
    int row_num = atoi(positions[0])-1;
    if(row_num < 0){
        fprintf(stderr, "ERR> Row number must be greater than 0!\n");
        return 1;
    }
    s->cell_min = 0;
    s->cell_max = table->rows[0].size-1;
    s->row_min = row_num;
    s->row_max = row_num;
    return 0;
}
int selector_cell(Table * table, Selection *s, char **positions){
    (void)(*table);
    int row_num = atoi(positions[0])-1;
    int cell_num = atoi(positions[1])-1;
    if(row_num < 0){
        fprintf(stderr, "ERR> Row number must be greater than 0!\n");
        return 1;
    }
    if(cell_num < 0){
        fprintf(stderr, "ERR> Column number must be greater than 0!\n");
        return 1;
    }
    s->cell_min = cell_num;
    s->cell_max = cell_num;
    s->row_min = row_num;
    s->row_max = row_num;
    return 0;
}
int selector_window(Table * table, Selection *s, char **positions){
    (void)(*table);
    int row_min = atoi(positions[0])-1;
    int cell_min = atoi(positions[1])-1;
    int row_max = atoi(positions[2])-1;
    int cell_max = atoi(positions[3])-1;
    if(row_min < 0 || row_max < 0 || row_min > row_max){
        fprintf(stderr, "ERR> Row number must be greater than 0!\n");
        fprintf(stderr, "ERR> Selector format [R1,C1,R2,C2] must have R1 <= R <= R2");
        return 1;
    }
    if(cell_min < 0 || cell_max < 0 || cell_min > cell_max){
        fprintf(stderr, "ERR> Column number must be greater than 0!\n");
        fprintf(stderr, "ERR> Selector format [R1,C1,R2,C2] must have C1 <= C <= C2");
        return 1;
    }
    s->cell_min = cell_min;
    s->cell_max = cell_max;
    s->row_min = row_min;
    s->row_max = row_max;
    return 0;
}
bool isnum(char *input){
    if(input == NULL)
        return false;
    int length = strlen (input);
    for (int i=0;i<length; i++)
        if (!isdigit(input[i]) && input[i] != '.' && input[i] != ',')
        {
            return false;
        }
    return true;
}
int selector_max(Table *table, Selection *s){
    float max_val;
    int max_c = -1;
    int max_r;
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            if(table->rows[i].cells[j].data == NULL || !isnum(table->rows[i].cells[j].data))
                continue;
            float current_val = strtof(table->rows[i].cells[j].data, NULL);
            if(max_c == -1 || max_val < current_val){
                max_val = current_val;
                max_c = j;
                max_r = i;
            }
        }
    }
    if(max_c == -1)
        return 1;
    s->cell_max = max_c;
    s->cell_min = max_c;
    s->row_max = max_r;
    s->row_min = max_r;
    return 0;
}
int selector_min(Table *table, Selection *s){
    float min_val;
    int min_c = -1;
    int min_r;
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            if(table->rows[i].cells[j].data == NULL || !isnum(table->rows[i].cells[j].data))
                continue;
            float current_val = strtof(table->rows[i].cells[j].data, NULL);
            if(min_c == -1 || min_val > current_val){
                min_val = current_val;
                min_c = j;
                min_r = i;
            }
        }
    }
    if(min_c == -1)
        return 1;
    s->cell_max = min_c;
    s->cell_min = min_c;
    s->row_max = min_r;
    s->row_min = min_r;
    return 0;
}
int selector_find(Table *table, Selection *s, char *str){
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            if(table->rows[i].cells[j].data == NULL)
                continue;
            if(strstr(table->rows[i].cells[j].data, str) != NULL) {
                s->cell_max = j;
                s->cell_min = j;
                s->row_max = i;
                s->row_min = i;
                break;
            }       
        }
    }
    return 0;
}
int selector_set(Vars * vars){
    //printf("Last sel was %s\n",vars->last_selection);
    strcpy(vars->saved_selection,vars->last_selection);
    return 0;
}
/*int selector_load(Table table, Selection * s, Vars * vars){
    //printf("Last sel was %s\n",vars->last_selection);
    
}*/
int selector_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    //printf("Running %s\n", self->flag);
    //[R,C] - výběr buňky na řádku R a sloupci C
    if(self->param[1] != 'm' && self->param[1] != 'f' && self->param[1] != 's' && strcmp(self->param, "[_]")){
        char param[1000];
        strncpy(param, self->param + 1, strlen(self->param) - 2);
        int pos_s = 0;
        char *positions[4]; 
        sequence_separate(positions, &pos_s, param, ",");
        //printf("Param is: %s\n",param);
        /*for (int i = 0; i < pos_s; i++)
        {
            printf("Pos %d is %s\n",i,positions[i]);
        }*/
        if(pos_s == 2){
            //[_,_] - výběr celé tabulky
            if (positions[0][0] == '_' && positions[1][0] == '_')
            {
                selector_whole(table, s);
            }//[_,C] - výběr celého sloupce C
            else if(positions[0][0] == '_'){
                selector_col(table, s, positions);
            }//[R,_] - výběr celého řádku R.
            else if(positions[1][0] == '_'){
                selector_row(table, s, positions);
            }//[R,C] - výběr buňky na řádku R a sloupci C
            else
            {
                selector_cell(table, s, positions);
            }
        }else if(pos_s == 4)
        {
            selector_window(table, s, positions);
        }else
        {
            fprintf(stderr,"Err> unknown selector!\n");
            return 1;
        }
    }else{
        if(strcmp(self->param, "[max]") == 0){
            selector_max(table, s);
        }else if(strcmp(self->param, "[min]") == 0){
            selector_min(table, s);
        }else if(self->param[1] == 'f')
        {
            char searched[1000];
            //strncpy(searched, self->param + 6, strlen(self->param) - 7);
            cfrom(searched, self->param, 6, strlen(self->param) - 2);
            selector_find(table, s, searched);
        }else if(strcmp(self->param,"[set]") == 0)
        {
            selector_set(vars);
        }else if(strcmp(self->param,"[_]") == 0){
            //printf("Exec, again: %s\n",vars->saved_selection);
            Cmd saved = {.action=selector_action,.flag="selector",.param=vars->saved_selection};
            return saved.action(&saved, table, s, vars);
        }else
        {
            fprintf(stderr,"Err> unknown selector!\n");
            return 1;
        }
        
    }
    strcpy(vars->last_selection,self->param);
    return 0;
}
int irow_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    Row new_row;
    row_ctor(&new_row);
    table_insert(table, new_row, s->row_min);
    table_align(table);
    return 0;
}
int arow_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    Row new_row;
    row_ctor(&new_row);
    table_insert(table, new_row, s->row_max+1);
    table_align(table);
    return 0;
}
int drow_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        table_remove(table, i);
    }
    return 0;
}
int icol_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    int col = s->cell_min;
    for (int i = 0; i < table->size; i++)
    {
        Cell new_cell;
        cell_ctor(&new_cell);
        row_insert(&table->rows[i], new_cell, col);
    }
    table_align(table);
    return 0;
}
int acol_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    int col = s->cell_max;
    for (int i = 0; i < table->size; i++)
    {
        Cell new_cell;
        cell_ctor(&new_cell);
        row_insert(&table->rows[i], new_cell, col+1);
    }
    table_align(table);
    return 0;
}
int dcol_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    for (int i = 0; i < table->size; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            printf("removing col %d\n",j+1);
            row_remove_dt(&table->rows[i], j);
        }
    }
    table_align(table);
    return 0;
}
int set_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            cell_set(&table->rows[i].cells[j], self->param);
        }
    }
    return 0;
}
int clear_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    (void)(*self);
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            cell_dtor(&table->rows[i].cells[j]);
        }
    }
    return 0;
}
int swap_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    char param[1000];
    cfrom(param, self->param, 1, strlen(self->param)-2);
    char *positions[2];
    int no_pos = 0;
    sequence_separate(positions, &no_pos, param, ",");
    int first_row = atoi(positions[0])-1;
    int first_cell = atoi(positions[1])-1;

    int second_row = s->row_min;
    int second_cell = s->cell_min;
    //insert 2nd to 1st
    row_insert(&table->rows[first_row], table->rows[second_row].cells[second_cell] ,first_cell);
    //insert 1st to second
    row_insert(&table->rows[second_row], table->rows[first_row].cells[first_cell+1] ,second_cell);
    //remove originals
    row_remove(&table->rows[second_row],second_cell+1);
    row_remove(&table->rows[first_row],first_cell+1);

    return 0;
}
int sum_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    int row;
    int cell;
    posfrompar(self->param, &row, &cell);
    long int sum = 0;
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            if(isnum(table->rows[i].cells[j].data)){
                sum += atoi(table->rows[i].cells[j].data);
            }
        }
    }
    char buf[12];
    //printf("%lf\n",sum);
    intToStr(sum, buf, 1);
    cell_set(&table->rows[row].cells[cell], buf);
    return 0;
}
int avg_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    int row;
    int cell;
    posfrompar(self->param, &row, &cell);
    double sum = 0;
    double n = 0;
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            if(isnum(table->rows[i].cells[j].data)){
                sum += strtof(table->rows[i].cells[j].data, NULL);
                n++;
            }
        }
    }
    int avg = sum/n;
    char buf[12];
    //printf("%lf\n",sum);
    intToStr(avg, buf, 1);
    cell_set(&table->rows[row].cells[cell], buf);
    return 0;
}
int count_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    int row;
    int cell;
    posfrompar(self->param, &row, &cell);
    int sum = 0;
    for (int i = s->row_min; i <= s->row_max; i++)
    {
        for (int j = s->cell_min; j <= s->cell_max; j++)
        {
            if((table->rows[i].cells[j].data) != NULL){
                sum++;
            }
        }
    }
    char buf[12];
    //printf("%lf\n",sum);
    intToStr(sum, buf, 1);
    cell_set(&table->rows[row].cells[cell], buf);
    return 0;
}
int len_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void)(*vars);
    int row;
    int cell;
    posfrompar(self->param, &row, &cell);
    int len = table->rows[s->row_min].cells[s->cell_min].size;
    char buf[12];
    //printf("%lf\n",sum);
    intToStr(len, buf, 1);
    cell_set(&table->rows[row].cells[cell], buf);
    return 0;
}
int def_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    char varnum_c = self->param[1];
    int varnum = varnum_c - '0';
    //printf("%s\n",table->rows[s->row_min].cells[s->cell_min].data);
    Cell c = table->rows[s->row_min].cells[s->cell_min];
    vars->list[varnum] = c.data;
    return 0;
}
int use_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    char varnum_c = self->param[1];
    int varnum = varnum_c - '0';
    //printf("%s\n",table->rows[s->row_min].cells[s->cell_min].data);
    Cell c = table->rows[s->row_min].cells[s->cell_min];
    cell_set(&c,  vars->list[varnum]);
    return 0;
}
int inc_action(Cmd * self,Table * table, Selection * s, Vars * vars){
    (void) (*table);
    (void) (*s);
    char varnum_c = self->param[1];
    int varnum = varnum_c - '0';
    if(isnum(vars->list[varnum])){
        int var_val = atoi(vars->list[varnum]);
        intToStr(var_val+1,vars->list[varnum],10);
    }
    return 0;
}

int main(int argc, char **argv)
{
    //DELIM & FILENAME PROCESSING
    char *delim = " ";
    char *filename = "";
    char *cmd_sequence = "";
    if(argc == 5){
        if(strcmp(argv[1],"-d") == 0){
            delim = argv[2];
            cmd_sequence = argv[3];
            filename = argv[4];
        }else{
            usage_print(stderr);
            return 1;
        }
    }else if(argc == 3){
        cmd_sequence = argv[1];
        filename = argv[2];
    }
    else{
        usage_print(stderr);
        return 1;
    }

    //printf("Delim is %s, Filename is %s, CMD_SEQ: %s\n",delim,filename,cmd_sequence);
    //COMMANDS INITIALIZATION
    //Cmd cmds[30];
    //int no_cmds = 0;
    Cmd_l cmds;
    cmds.size = 0;
    INIT_CMD(selector);  //MUST BE INITIALIZED FIRST
    INIT_CMD(irow);
    INIT_CMD(arow);
    INIT_CMD(drow);
    INIT_CMD(icol);
    INIT_CMD(acol);
    INIT_CMD(dcol);
    INIT_CMD(set);
    INIT_CMD(clear);
    INIT_CMD(swap);
    INIT_CMD(sum);
    INIT_CMD(avg);
    INIT_CMD(count);
    INIT_CMD(len);
    INIT_CMD(def);
    INIT_CMD(use);
    INIT_CMD(inc);

    char *cmd_sequence_sep[MAX];
    int cmd_sequence_size = 0;
    //CMD PARAMETERS PROCESSING
    Cmd_l cmd_queue;
    cmd_queue.size = 0;
    sequence_separate(cmd_sequence_sep, &cmd_sequence_size , cmd_sequence, ";");
    sequence_queue(&cmds, &cmd_queue, cmd_sequence_sep, cmd_sequence_size);
    /*for (int i = 0; i < cmd_queue.size; i++)
    {
        printf("Cmd %d is %s, param: %s\n",i,cmd_queue.list[i].flag,cmd_queue.list[i].param);
    }*/
    //load file
    FILE *file = fopen(filename,"r+");
    if(file == NULL){
        fprintf(stderr,"ERR> File is NULL!\n");
        return 1;
    }
    //LOADING TABLE
    Table table;
    table_ctor(&table);
    table_load(&table, file, delim);
    table_align(&table);

    //VARIABLES
    //Vars * vars;
    Vars v = {.list = {'\0'}, .saved_selection = "", .last_selection = ""};

    //COMMANDS EXECUTION
    Selection actual_selector;
    actual_selector.cell_min = actual_selector.cell_max = actual_selector.row_min = actual_selector.row_max = 0;
    for (int i = 0; i < cmd_queue.size; i++)
    {
        int res = cmd_queue.list[i].action(&cmd_queue.list[i], &table, &actual_selector, &v);
        //printf("[%d] Result of %s, was %d\n", i, cmd_queue.list[i].flag, res);
        if(res != 0){
            fprintf(stderr, "Error %d, <>\n",res);
            break;
        }
    }
    
    table_print(table);
    fclose(file);
    file = fopen(filename, "w");
    for (int i = 0; i < table.size; i++)
    {
        for (int j = 0; j < table.rows[i].size; j++)
        {
            fprintf(file, table.rows[i].cells[j].data);
            if(j + 1 < table.rows[i].size)
                fputc(delim[0],file);
        }
        if(i + 1 < table.size)
                fputc('\n',file);
    }

    //EXITING
    for (int i = 0; i < table.size; i++)
    {
        for (int j = 0; j < table.rows[i].size; j++)
        {
            cell_dtor(&table.rows[i].cells[j]);
        }
        row_dtor(&table.rows[i]);
    }
    
    table_dtor(&table);
    fclose(file);
    return 0;
}