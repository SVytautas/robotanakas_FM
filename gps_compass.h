#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GPS_RX_BUFF_SIZE 512
#define GPS_RX_MAX_DATA 96 //96 baitai jei reikes daugiau padidinsma
#define NAV_POSLLH_HEADER (0xB5<<24 | 0x62<<16 | 0x01<<8 | 0x02)
#define NAV_STATUS_HEADER (0xB5<<24 | 0x62<<16 | 0x01<<8 | 0x03)
#define NAV_VELNED_HEADER (0xB5<<24 | 0x62<<16 | 0x01<<8 | 0x12)

#define CHECK_BIT(var,pos) ((var) & (1<<(pos)))



//compass
#define COMP_PI      		 3.14159265358979323846f
#define HMC5983_ADDRESS 0x1E

#define HMC5983L_ADDRESS              (0x1E)
#define HMC5983L_REG_CONFIG_A         (0x00)
#define HMC5983L_REG_CONFIG_B         (0x01)
#define HMC5983L_REG_MODE             (0x02)
#define HMC5983L_REG_OUT_X_M          (0x03)
#define HMC5983L_REG_OUT_X_L          (0x04)
#define HMC5983L_REG_OUT_Z_M          (0x05)
#define HMC5983L_REG_OUT_Z_L          (0x06)
#define HMC5983L_REG_OUT_Y_M          (0x07)
#define HMC5983L_REG_OUT_Y_L          (0x08)
#define HMC5983L_REG_STATUS           (0x09)
#define HMC5983L_REG_IDENT_A          (0x0A)
#define HMC5983L_REG_IDENT_B          (0x0B)
#define HMC5983L_REG_IDENT_C          (0x0C)

// MEMORY MAPPING
/*
Address Location  Name    Access
---------------------------------------------------
0x00  Configuration Register A  Read/Write
0x01  Configuration Register B  Read/Write
0x02  Mode Register     Read/Write
0x03  Data Output X MSB Register  Read
0x04  Data Output X LSB Register  Read
0x05  Data Output Z MSB Register  Read
0x06  Data Output Z LSB Register  Read
0x07  Data Output Y MSB Register  Read
0x08  Data Output Y LSB Register  Read
0x09  Status Register     Read
0x0A  Identification Register A Read
0x0B  Identification Register B Read
0x0C  Identification Register C Read
0x31  Temperature Output MSB Register Read
0x32  Temperature Output LSB Register Read
*/
typedef enum {   
	  HMC5983L_SAMPLES_8     = 3,
    HMC5983L_SAMPLES_4     = 2,
    HMC5983L_SAMPLES_2     = 1,
    HMC5983L_SAMPLES_1     = 0
}hmc5983l_samples_t;

typedef enum
{
    HMC5983L_RANGE_8_1GA     = 7,  //0b111,
    HMC5983L_RANGE_5_6GA     = 6,   //0b110,
    HMC5983L_RANGE_4_7GA     = 5,  //0b101,
    HMC5983L_RANGE_4GA       = 4,  //0b100,
    HMC5983L_RANGE_2_5GA     = 3,  //0b011,
    HMC5983L_RANGE_1_9GA     = 2, //0b010,
    HMC5983L_RANGE_1_3GA     = 1, //0b001,
    HMC5983L_RANGE_0_88GA    = 0//0b000
} hmc5983l_range_t;

typedef enum
{
    HMC5983L_IDLE          = 2,//0b10,
    HMC5983L_SINGLE        = 1,//0b01,
    HMC5983L_CONTINOUS     = 0,//0b00
} hmc5983l_mode_t;

typedef enum
{
	  HMC5983L_DATARATE_220HZ   =    7,//0b111,
    HMC5983L_DATARATE_75HZ       = 6,//0b110,
    HMC5983L_DATARATE_30HZ       = 5,//0b101,
    HMC5983L_DATARATE_15HZ       = 4,//0b100, //default
    HMC5983L_DATARATE_7_5HZ      = 3,//0b011,
    HMC5983L_DATARATE_3HZ        = 2,//0b010,
    HMC5983L_DATARATE_1_5HZ      = 1,//0b001,
    HMC5983L_DATARATE_0_75_HZ    = 0,//0b000
	  
} hmc5983l_dataRate_t;


typedef struct
{
	float mgPerDigit; 
	uint8_t state;
	uint16_t delay;
	int16_t xaxis;
	int16_t yaxis;
	int16_t zaxis;
	float xnorm;
	float ynorm;
	float xOffset;
	float yOffset;
	float heading;
	float headingdeg;
	bool  calibrating;
	int16_t minX;
	int16_t maxX;
	int16_t minY;
	int16_t maxY;
}compass_t;


enum {
COMPASS_ST_MSG_NONE=0,
COMPASS_ST_INIT,
COMPASS_ST_READ_XYZ,
COMPASS_ST_CALIBRATE,

};



enum {
GPS_ST_MSG_NONE=0,
GPS_ST_MSG_POSLLH,
GPS_ST_MSG_STATUS,
GPS_ST_MSG_VELNED,

};

typedef struct
{
	uint32_t header;
  bool reading;
	bool start;
	uint8_t gps_data[GPS_RX_MAX_DATA];
	uint8_t rx_buffer[GPS_RX_BUFF_SIZE];
	uint16_t rx_buffer_wp;
	uint16_t rx_buffer_rp;
	uint16_t pointer_start;
	uint8_t  msg_byte_count;
	uint8_t msg_parse;
	int32_t   lon; //POSLLH
	int32_t   lat; //POSLLH
	uint32_t hAcc; //POSLLH Horizontal accuracy estimate mm
	uint8_t  gpsFix;  //STATUS
											/*GPSfix Type, range 0..5
											0x00 = No Fix
											0x01 = Dead Reckoning only
											0x02 = 2D-Fix
											0x03 = 3D-Fix
											0x04 = GPS + dead reckoning combined
											0x05 = Time only fix
											0x06..0xff: reserved */

	uint8_t  flags;  //STATUS//Navigation Status Flags  (0 bitas position valid jeigu 1)
	uint32_t gSpeed; //NAV-VELNED Ground speed (2-D) cm/s
  float 	 heading; //NEV-VELNED Heading of motion 2-D
	
}gps_t;


void gps_compass_init(void);
void gps_rewrite_dma(void);
void gps_handler(void);
uint16_t gps_cacl_crc( uint8_t msg_size);
void gps_print_nokia(void);

//compass
void gps_compass_handler(void);
void gps_nokia_print_compass(void);

uint8_t gps_read_register_comp(uint8_t reg);
void gps_write_register_comp(uint8_t reg, uint8_t data);

void HMC5983L_setRange(hmc5983l_range_t range);
hmc5983l_range_t HMC5983L_getRange(void);
void HMC5983L_setMeasurementMode(hmc5983l_mode_t mode);
hmc5983l_mode_t HMC5983L_getMeasurementMode(void);
void HMC5983L_setDataRate(hmc5983l_dataRate_t dataRate);
hmc5983l_dataRate_t HMC5983L_getDataRate(void);
void HMC5983L_setSamples(hmc5983l_samples_t samples);
hmc5983l_samples_t HMC5983L_getSamples(void);

void gps_nokia_print_compass_calib(void);
void gps_compass_set_calibrate(void);
void gps_compass_reset_calibrat(void);
void gps_compass_set_calib(void);

float get_gps_compass_headingdeg(void);