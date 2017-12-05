#include <stdint.h>
#include <stdbool.h>

#define DSTAR_MAP_SIZE 12 //9 //9x9 matrica
#define MAX_OPEN_LIST  144  //81 //9x9 matrica

#define DSTAR_FREE 0xFFFF
#define DSTAR_OBSTACLE 0xFFF0

#define DSTAR_FORWARD 500 //mm
#define DSTAT_DIAGNOL 700 //mm

#define DSTAT_MAX_CYCLE 1000 //mm

#define OPEN_PATH_SIZE 64

typedef struct
{
	uint16_t map_g[DSTAR_MAP_SIZE][DSTAR_MAP_SIZE];
	uint16_t map_rhs[DSTAR_MAP_SIZE][DSTAR_MAP_SIZE];
	uint8_t goal_x;
	uint8_t goal_y;
	uint8_t open_x[MAX_OPEN_LIST];
	uint8_t open_y[MAX_OPEN_LIST];
	uint8_t open_st[MAX_OPEN_LIST];
	uint8_t start_x;
	uint8_t start_y;
	uint16_t count_no;
	uint8_t open_path_x[OPEN_PATH_SIZE];
	uint8_t open_path_y[OPEN_PATH_SIZE];
	float   opened_atstumas[OPEN_PATH_SIZE];
	float   opened_kampas[OPEN_PATH_SIZE];
	uint8_t open_path_st[OPEN_PATH_SIZE];
	float   kampas_start;
	float   command_atstumas[OPEN_PATH_SIZE];
  float   command_kampas[OPEN_PATH_SIZE];
	uint8_t command_count;
}dstar_t;


enum 
{
DSTAR_ONPEN_NO=0,
DSTAR_ONPEN_YES,	

};

enum 
{
DSTAR_PLAN_GOAL_OBSTACLE=0,
DSTAR_PLAN_STAR_SURAUND_OBSTACLE,
DSTAR_PLAN_OK,
DSTAR_PLAN_NOLIST,
DSTAR_PLAN_CYCLE_ER,	

};

enum 
{
DSTAR_PROC_NOLIST=0,
DSTAR_PROC_LIST,
DSTAR_PROC_START,	

};

enum 
{
DSTAR_PATH_ERROR=0,
DSTAR_PATH_OK,
DSTAR_PATH_PLAN,	
};

void dstar_printf_map(void);
void dstar_reset_map(void);
void dstar_add_obstacle(uint16_t x,uint16_t y);

void dstar_open_path(uint8_t x, uint8_t y,float atstumas, float kampas);
uint8_t dstar_make_path(void);
uint8_t dstar_plan_command(float start_kampas);

void dstar_open(uint8_t x, uint8_t y);
uint8_t find_min_open_and_procead(void);
uint8_t dstar_plan(uint8_t goalx, uint8_t goaly, uint8_t start_x, uint8_t start_y);
void dstar_printf_path(void);
void dstar_push_command_to_ZV(void);
void dstar_set_kampas_start(float kampas_startass);