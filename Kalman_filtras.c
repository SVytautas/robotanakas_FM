/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "arm_math.h"
#include "stdio.h"
#include "stdlib.h"


float32_t Fx_f32[9];
float32_t Fu_f32[6];
float32_t P_f32[9];
float32_t FxT_f32[9];
float32_t FuT_f32[6];
float32_t A_f32[9];
float32_t AA_f32[9];
float32_t B_f32[6];
float32_t BB_f32[9];

//matricos matavimo algoritmui
float32_t H_f32[6];
float32_t V_f32[4];
float32_t HT_f32[6];
float32_t VTr_f32[4];
float32_t S_f32[4];
float32_t SS_f32[4];
float32_t SINV_f32[4];
float32_t vt_f32[2];

float32_t K_f32[6];
float32_t KT_f32[6];
float32_t J_f32[3];

//tarpines
float32_t N_f32[6];
float32_t NN_f32[4];

float32_t M_f32[4];
float32_t MM_f32[4];

float32_t L_f32[6];
float32_t W_f32[6];
float32_t WW_f32[9];

float32_t xxx[3];
float32_t b=100;
float32_t trikst,trikkam;
	
//kintamieji matavimo
float32_t skirx,skiry,skirxk,skiryk;
float32_t zz[2];



//kintamieji
 float32_t xt[3] =
{100, 0, 0};

float32_t xt2[3] =
{100, 0, 0};

float32_t PP_f32[9] =
{// Const,   numTaps,   blockSize,   numTaps*blockSize 
  0,     0,      0,  
  0,     0,      0,    
  0,     0,      0,    };
float32_t zyme_f32[2] =
{100,1050,};

float32_t R_f32[4] =
{200,  0,  
  0 , 0.5};
 float32_t Q_f32[4] =
{0,  0,  
  0 , 0};
	
	
float32_t u1[361]={
19.312,19.028,19.283,19.034,19.215,19.173,19.294,19.06,19.306,19.222,19.117,19.346,19.228,19.207,19.333,19.365,19.034,19.318,19.38,19.181,19.357,19.352,19.299,19.262,19.282,19.156,19.241,19.281,19.266,19.319,19.151,19.159,19.284,19.03,19.122,19.063,19.226,19.134,19.347,19.158,19.364,19.234,19.211,19.378,19.14,19.222,19.287,19.304,19.37,19.156,19.094,19.037,19.226,19.028,19.174,19.338,19.205,19.301,19.201,19.231,19.205,19.176,19.329,19.148,19.163,19.161,19.145,19.171,19.052,19.308,19.091,19.11,19.353,19.151,19.174,19.172,19.268,19.09,19.349,19.24,19.134,19.019,19.136,19.217,19.099,19.366,19.145,19.275,19.117,19.228,19.2,19.083,19.105,19.37,19.264,19.18,19.256,19.051,19.235,19.138,19.065,19.174,19.227,19.225,19.092,19.201,19.374,19.041,19.275,19.318,19.13,19.22,19.31,19.049,19.38,19.333,19.041,19.03,19.08,19.267,19.264,19.189,19.027,19.31,19.341,19.036,19.108,19.307,19.17,19.144,19.081,19.37,19.198,19.376,19.191,19.221,19.212,19.123,19.214,19.013,19.105,19.114,19.345,19.379,19.151,19.319,19.236,19.289,19.126,19.185,19.253,19.087,19.01,19.306,19.026,19.235,19.336,19.091,19.299,19.252,19.281,19.346,19.25,19.315,19.055,19.24,19.346,19.087,19.054,19.023,19.115,19.356,19.081,19.167,19.247,19.032,19.297,19.067,19.365,19.23,19.02,19.021,19.062,19.131,19.155,19.079,19.302,19.273,19.119,19.132,19.22,19.096,19.213,19.383,19.026,19.336,19.164,19.086,19.043,19.215,19.219,19.144,19.246,19.055,19.166,19.148,19.1,19.141,19.026,19.329,19.39,19.089,19.095,19.376,19.009,19.096,19.324,19.133,19.231,19.161,19.387,19.266,19.16,19.047,19.143,19.276,19.156,19.227,19.246,19.04,19.041,19.175,19.245,19.331,19.192,19.386,19.276,19.38,19.161,19.221,19.192,19.083,19.161,19.389,19.02,19.376,19.208,19.214,19.206,19.133,19.126,19.032,19.345,19.238,19.382,19.186,19.095,19.237,19.204,19.183,19.316,19.027,19.375,19.135,19.201,19.071,19.368,19.247,19.286,19.012,19.362,19.251,19.028,19.273,19.276,19.039,19.312,19.37,19.035,19.358,19.065,19.111,19.325,19.226,19.312,19.185,19.354,19.107,19.054,19.231,19.284,19.258,19.115,19.132,19.125,19.059,19.198,19.275,19.019,19.332,19.367,19.015,19.05,19.034,19.31,19.1,19.275,19.205,19.052,19.377,19.186,19.296,19.109,19.286,19.105,19.109,19.033,19.085,19.199,19.096,19.205,19.198,19.359,19.082,19.159,19.273,19.12,19.235,19.007,19.341,19.057,19.359,19.162,19.239,19.386,19.165,19.314,19.203,19.208,19.079,19.252,19.205,19.299,19.039,19.329,19.074,19.024,19.174,19.16,19.132,19.101,19.278,19.039,19.296,19.268,19.221,19.144,19.363,19.172,19.076,19.15
};
float32_t u2[361]={
17.469,17.385,17.531,17.601,17.623,17.443,17.455,17.445,17.512,17.384,17.558,17.608,17.288,17.409,17.409,17.512,17.421,17.448,17.624,17.365,17.474,17.436,17.587,17.322,17.378,17.453,17.479,17.587,17.432,17.392,17.47,17.459,17.418,17.308,17.283,17.499,17.627,17.429,17.301,17.493,17.345,17.455,17.349,17.495,17.459,17.521,17.404,17.535,17.553,17.485,17.57,17.512,17.321,17.298,17.569,17.313,17.329,17.575,17.374,17.545,17.338,17.608,17.465,17.362,17.318,17.57,17.328,17.369,17.452,17.305,17.279,17.34,17.607,17.462,17.339,17.614,17.525,17.548,17.578,17.596,17.577,17.465,17.344,17.296,17.364,17.565,17.345,17.497,17.454,17.54,17.4,17.513,17.282,17.595,17.571,17.482,17.533,17.299,17.615,17.604,17.417,17.311,17.554,17.3,17.571,17.376,17.495,17.368,17.532,17.42,17.457,17.464,17.39,17.317,17.527,17.597,17.571,17.483,17.433,17.351,17.478,17.596,17.56,17.406,17.394,17.484,17.544,17.347,17.533,17.304,17.43,17.545,17.46,17.516,17.36,17.543,17.615,17.481,17.43,17.558,17.408,17.348,17.443,17.421,17.585,17.592,17.594,17.612,17.304,17.282,17.279,17.438,17.533,17.431,17.296,17.363,17.615,17.358,17.4,17.599,17.48,17.416,17.497,17.628,17.36,17.317,17.47,17.433,17.443,17.52,17.325,17.492,17.542,17.333,17.537,17.611,17.538,17.438,17.57,17.482,17.588,17.539,17.49,17.419,17.492,17.312,17.36,17.566,17.387,17.569,17.371,17.438,17.625,17.361,17.543,17.624,17.279,17.355,17.54,17.397,17.613,17.469,17.557,17.566,17.534,17.539,17.324,17.379,17.486,17.337,17.403,17.418,17.606,17.432,17.492,17.604,17.479,17.357,17.568,17.58,17.35,17.366,17.488,17.566,17.429,17.538,17.429,17.575,17.484,17.453,17.595,17.552,17.37,17.347,17.397,17.334,17.41,17.503,17.499,17.605,17.502,17.417,17.509,17.507,17.495,17.539,17.369,17.289,17.299,17.359,17.359,17.375,17.434,17.552,17.575,17.392,17.481,17.435,17.421,17.436,17.523,17.355,17.555,17.299,17.349,17.541,17.285,17.467,17.312,17.381,17.303,17.506,17.564,17.353,17.613,17.299,17.52,17.46,17.351,17.552,17.575,17.358,17.566,17.379,17.433,17.376,17.596,17.519,17.345,17.304,17.602,17.589,17.619,17.618,17.321,17.395,17.494,17.289,17.567,17.365,17.295,17.517,17.557,17.303,17.311,17.315,17.535,17.393,17.33,17.617,17.508,17.474,17.542,17.421,17.465,17.366,17.383,17.501,17.506,17.488,17.571,17.376,17.457,17.318,17.426,17.311,17.386,17.377,17.378,17.294,17.57,17.327,17.339,17.354,17.45,17.291,17.4,17.407,17.56,17.322,17.284,17.469,17.577,17.535,17.409,17.324,17.518,17.411,17.432,17.388,17.387,17.329,17.44,17.523,17.473,17.594,17.449,17.442,17.524,17.578,17.356
};
float32_t zt[361]={
958.06,1032.5,880.2,1057.5,1030,1036.1,982.74,1001,964.03,890.79,1086.6,1061.8,998.68,975.19,1079.7,1013.1,928.21,1030.3,1026.9,1046.7,1051.6,985.34,937.06,944.35,953.19,1071.5,1031.8,944.26,1003.6,952.99,924.41,988,1029.1,1029.8,907.7,971.83,1070.4,991.11,961.61,980.13,1058.6,947.53,1014.9,968.16,1027.3,977.49,978.57,966.61,973.82,1004.3,954.56,897.94,968.99,1017.6,894.37,1003.8,1043.4,971.99,1082.4,902.03,904.07,1069.8,1008.6,997.7,1010.1,961.25,956.89,917.57,937.49,1010,940.15,905.94,912.77,924.71,917.79,903.25,1011.1,1010.6,921.91,953.88,1064.1,984.38,1009.3,941.46,981.42,928.87,1011.5,875.76,986.03,1042.1,993.33,887.38,974.09,977.62,940.74,1053.4,1025.5,958.39,1064.8,907.14,949.88,941.35,982.86,926.2,984.85,1087.2,1017.5,986.56,1067.5,929.88,1058.1,901.95,1059.7,980.43,935.82,935.26,997.44,1014.5,950.18,971.64,930.52,902.37,971.73,965.91,994.69,1028.6,955.77,1063.6,1089.5,991.79,1014.2,1009.9,978.99,1092.2,977.81,967.55,1097,888.43,1001.9,996.76,909.15,998.34,913.86,948.85,1052.4,1034.4,959.73,1014.6,989.98,902.73,1118.3,1079.3,1083.6,1093.6,1097.2,1082,1115,1099.5,1101.6,1098.3,1085.1,1097.1,1085.9,1092.4,1121.2,1079,1095.9,1094.2,1120.1,1115.7,1121.1,1108.2,1117.6,1093.2,1114,1113.4,1099.9,1114.6,1105.2,1117.4,1115.6,1095.9,1084.8,1089.2,1095.9,1085.3,1092.2,1110.6,1114.4,1101,1113.7,1107.9,1094.9,1111.2,1101.3,1104.5,1118.9,1101.8,1092.3,1110.9,1114.6,1117.3,1093.3,1110.8,1079.1,1112.4,1117.3,1087.9,1118.8,1085.2,1100.1,1080.1,1092.7,1108.1,1119.4,1113.2,1111.6,1112.9,1091.7,1090.8,1105,1114.4,1098.9,1113.2,1115,1103.2,1111.3,1120.1,1090.2,1120.4,1100.9,1116.9,1084.5,1097.6,1091.4,1113.1,1088.4,1120.8,1115.8,1121.4,1109.7,1117.1,1121.7,1117.7,1082.8,1103,1107.2,1120.3,1108.6,1117.2,1083,1106.7,1090.4,1111.3,1083,1080.2,1105.7,1104.5,1079.6,1082.8,1102.3,1116.4,1098.2,1097.9,1110.6,1096.8,1094.2,1114.5,1106.6,1116.6,1085.9,1103.6,1116,1101.3,1101.9,1097.6,1105.7,1093.2,1120,1112.1,1091,1112.5,1092.1,1103.1,1108.8,1097.6,1107.7,1110.9,1083.8,1084.4,1114.2,1099.7,1101.7,1079.6,1094.1,1118.3,1117.5,1103.7,1101.2,1093,1103.6,1080.4,1104.3,1105.2,1084,1088.5,1115.8,1106.6,1107.1,1078.9,1083.4,1090.8,1096.8,1117.5,1119.3,1120,1118.8,1104,1113.1,1113,1082.9,1118.5,1106.7,1120.9,1091.1,1091.7,1095.7,1082.6,1084.8,1102.2,1117.8,1113.2,1100.2,1103.3,1100.9,1108.6,1090.9,1081.7,1110.5,1113.9,1114.1,1117.4,1104.7,1094.8,1092.6,1114.4,1116.7,1110.3,1121.1,1084.9,1117.4,1096,1116.3,1079.3,1087.4,1115.4,1096.5,1089.3,1089.2,1095.7,1103.7
};

float32_t XKalman[361];
float32_t YKalman[361];
float32_t kamKalman[361];
float32_t nnn;
/* ----------------------------------------------------------------------
* Temporary buffers  for storing intermediate values
* ------------------------------------------------------------------- */
//float32_t Xmat[361];
//float32_t Ymat[361];
//int	i=0;
//int j=0;
	//matricu nustatimai
	arm_matrix_instance_f32 Fx;
  arm_matrix_instance_f32 FxT; 	
  arm_matrix_instance_f32 PP;
  arm_matrix_instance_f32 Fu;
	arm_matrix_instance_f32 FuT;
	arm_matrix_instance_f32 Q;
	arm_matrix_instance_f32 P;
	//tarpine matricos veiksmams
	arm_matrix_instance_f32 A;
	arm_matrix_instance_f32 AA;
	arm_matrix_instance_f32 B;
	arm_matrix_instance_f32 BB;
	//matavimo matricos
	arm_matrix_instance_f32 H;
	arm_matrix_instance_f32 V;
	arm_matrix_instance_f32 HT;
	arm_matrix_instance_f32 VTr;
  arm_matrix_instance_f32 R;
  arm_matrix_instance_f32 S;
	 arm_matrix_instance_f32 SS;
	arm_matrix_instance_f32 K;
	arm_matrix_instance_f32 KT;
	arm_matrix_instance_f32 SINV;
	arm_matrix_instance_f32 J;
	arm_matrix_instance_f32 vt;
	
	//tarpines matricos
	arm_matrix_instance_f32 N;
	arm_matrix_instance_f32 NN;
	arm_matrix_instance_f32 M;
	arm_matrix_instance_f32 MM;
	arm_matrix_instance_f32 L;
	arm_matrix_instance_f32 W;
	arm_matrix_instance_f32 WW;
	
uint32_t kk=0;


void Kalman_Init(void)
{

	
//spejimo matricos
uint32_t srcRows, srcColumns;  /* Temporary variables */
srcRows = 3;
srcColumns = 3;
arm_mat_init_f32(&Fx, srcRows, srcColumns, Fx_f32);
arm_mat_init_f32(&FxT, srcRows, srcColumns, FxT_f32);
arm_mat_init_f32(&P, srcRows, srcColumns, P_f32);	
arm_mat_init_f32(&PP, srcRows, srcColumns, PP_f32);
arm_mat_init_f32(&BB, srcRows, srcColumns, BB_f32);
arm_mat_init_f32(&A, srcRows, srcColumns, A_f32);	
arm_mat_init_f32(&AA, srcRows, srcColumns, AA_f32);

arm_mat_init_f32(&WW, srcRows, srcColumns, WW_f32);

srcRows = 3;
srcColumns = 2;	
arm_mat_init_f32(&Fu, srcRows, srcColumns, Fu_f32);
arm_mat_init_f32(&B, srcRows, srcColumns, B_f32);
//matavimo matrica
arm_mat_init_f32(&HT, srcRows, srcColumns, HT_f32);
arm_mat_init_f32(&L, srcRows, srcColumns, L_f32);
arm_mat_init_f32(&K, srcRows, srcColumns, K_f32);
arm_mat_init_f32(&W, srcRows, srcColumns, W_f32);
 
 
srcRows = 2;
srcColumns = 3;
arm_mat_init_f32(&FuT, srcRows, srcColumns, FuT_f32);
//matavimo matricos
arm_mat_init_f32(&H, srcRows, srcColumns, H_f32);
arm_mat_init_f32(&N, srcRows, srcColumns, N_f32);
arm_mat_init_f32(&KT, srcRows, srcColumns, KT_f32);

srcRows = 2;
srcColumns = 2;
arm_mat_init_f32(&Q, srcRows, srcColumns, Q_f32);
//matavimo matricos
arm_mat_init_f32(&V, srcRows, srcColumns, V_f32);
arm_mat_init_f32(&VTr, srcRows, srcColumns, VTr_f32);
arm_mat_init_f32(&M, srcRows, srcColumns, M_f32);
arm_mat_init_f32(&MM, srcRows, srcColumns, MM_f32);
arm_mat_init_f32(&NN, srcRows, srcColumns, NN_f32);
arm_mat_init_f32(&R, srcRows, srcColumns, R_f32);
arm_mat_init_f32(&S, srcRows, srcColumns, S_f32);
arm_mat_init_f32(&S, srcRows, srcColumns, SS_f32);
arm_mat_init_f32(&SINV, srcRows, srcColumns, SINV_f32);
//matavimo matricos	
srcRows = 3;
srcColumns = 1;
arm_mat_init_f32(&J, srcRows, srcColumns, J_f32);	
srcRows = 2;
srcColumns = 1;
arm_mat_init_f32(&vt, srcRows, srcColumns, vt_f32);	

xt2[0]=0;	
xt2[1]=0;
xt2[2]=1.5708;	
//k=0;

}

void Kalman_handler(void)
{
	int k=0;
	for (k=0;k<360; k++)
	{
	
//Xmat[k]=zyme_f32[0]+zt[k]*sin((k*3.14)/180);
//Ymat[k]=zyme_f32[1]+zt[k]*cos((k*3.14)/180);
		
	Q_f32[0]=u1[k];
	Q_f32[3]=u2[k];


//spejimas
  /*  xt2- matrica roboto busenos t-1 momentu (x,y,kam) */
  /*  Pt2- klaidos koreliacijos matrica t-1 momentu */
  /*  u- valdimas sr ir sl */
  /*  Q- valdimo klaidos matrica */
  /* roboto tarp ratu atstumas {mm} */

		
trikst=(u1[k]+u2[k])/2;
trikkam=(u1[k]-u2[k])/b;

xt[0]=xt2[0]+ trikst*cosf(xt2[2]+trikkam);
xt[1] = xt2[1] + trikst*sinf(xt2[2]+trikkam);
xt[2] = xt2[2] + trikkam;	

  Fx_f32[0] = 1.0;
  Fx_f32[1] = 0.0;
  Fx_f32[2] = -trikst * sinf(xt2[2] + trikkam );
	
  Fx_f32[3] = 0.0;
  Fx_f32[4] = 1.0;
  Fx_f32[5] = trikst * cosf(xt2[2] + trikkam );
	
	Fx_f32[6] = 0.0;
  Fx_f32[7] = 0.0;
  Fx_f32[8] = 1.0;
	
	Fu_f32[0] = 0.5 * cosf(xt2[2] + trikkam );
  Fu_f32[1] = 0.5 * sinf(xt2[2] + trikkam );
  Fu_f32[2] = 0.5 * cosf(xt2[2] + trikkam );
  Fu_f32[3] = 0.5 * sinf(xt2[2] + trikkam );
	Fu_f32[4] = 1/b;
	Fu_f32[5] = 1/b;
	
	//FxT=Fx'
	arm_mat_trans_f32(&Fx, &FxT);
	//FuT=Fu'
	arm_mat_trans_f32(&Fu, &FuT);
	//A=Fx*PP
	arm_mat_mult_f32(&Fx, &PP, &A);
	//AA=Fx'*A
	arm_mat_mult_f32(&A, &FxT, &AA);
	//B=Fu*Q
	arm_mat_mult_f32(&Fu, &Q, &B);
	//BB=Fu'*B
	arm_mat_mult_f32(&B, &FuT, &BB);
	//P=Fx*Pt2*Fx'+Fu*Q*Fu';
	//P=AA+BB;
	arm_mat_add_f32(&AA, &BB, &P);
	
// matavimas

skirx=-zyme_f32[0]+xt[0];
skiry=-zyme_f32[1]+xt[1];
skirxk=skirx*skirx;
skiryk=skiry*skiry;
nnn=skiry/skirx;

zz[0]=sqrt(skirxk+skiryk);
//zz[1]=atan(nnn)-xt[2];
zz[1]=0;

H_f32[0]=2*skirx/(2*sqrt(skirxk + skiryk));
H_f32[1]=2*skiry/(2*sqrt(skirxk + skiryk));
H_f32[2]=0;
H_f32[3]=skiry/(skirxk*((skiryk/skirxk)+1));
H_f32[4]=1/(skirxk*((skiryk/skirxk)+1));
H_f32[5]=-1;


V_f32[0]=1;
V_f32[1]=0;
V_f32[2]=0;
V_f32[3]=1;

	//HT=H'
	arm_mat_trans_f32(&H, &HT);
	//VT=V'
	arm_mat_trans_f32(&V, &VTr);
	//N=H*P
	arm_mat_mult_f32(&H, &P, &N);
	//NN=N*H'
	arm_mat_mult_f32(&N, &HT, &NN);
	//M=V*R
	arm_mat_mult_f32(&V, &R, &M);
	//MM=M*V'
	arm_mat_mult_f32(&M, &VTr, &MM);
	//S=NN+MM;
	arm_mat_add_f32(&MM, &NN, &S);
	arm_mat_add_f32(&MM, &NN, &SS);

vt_f32[0]=zt[k]-zz[0];
vt_f32[1]=0-zz[1];

//L=P*H'
arm_mat_mult_f32(&P, &HT, &L);
//inv(S)
arm_mat_inverse_f32(&S, &SINV);
//K=L*Sinv
arm_mat_mult_f32(&L, &SINV, &K);


//J=K*vt;
arm_mat_mult_f32(&K, &vt, &J);

xxx[0]=xt[0]+J_f32[0];
XKalman[k]=xxx[0];
xxx[1]=xt[1]+J_f32[1];
YKalman[k]=xxx[1];
xxx[2]=xt[2]+J_f32[2];

xt2[0]=xt[0];
xt2[1]=xt[1];
xt2[2]=xt[2];

//K'
arm_mat_trans_f32(&K, &KT);
//W=K*S
arm_mat_mult_f32(&K, &SS, &W);
//WW=Q*K'
arm_mat_mult_f32(&W, &KT, &WW);
//PP=P-QQ
arm_mat_sub_f32(&P, &WW, &PP);
}

}



void Kalman_calc(int32_t Urs, int32_t Uls, uint32_t lidar_l,uint32_t lidar_kampas)
{
	
		Q_f32[0]=Urs;
	Q_f32[3]=Uls;


//spejimas
  /*  xt2- matrica roboto busenos t-1 momentu (x,y,kam) */
  /*  Pt2- klaidos koreliacijos matrica t-1 momentu */
  /*  u- valdimas sr ir sl */
  /*  Q- valdimo klaidos matrica */
  /* roboto tarp ratu atstumas {mm} */

		
trikst=(Urs+Uls)/2;
trikkam=(Urs-Uls)/b;

xt[0]=xt2[0]+ trikst*cosf(xt2[2]+trikkam);
xt[1] = xt2[1] + trikst*sinf(xt2[2]+trikkam);
xt[2] = xt2[2] + trikkam;	

  Fx_f32[0] = 1.0;
  Fx_f32[1] = 0.0;
  Fx_f32[2] = -trikst * sinf(xt2[2] + trikkam );
	
  Fx_f32[3] = 0.0;
  Fx_f32[4] = 1.0;
  Fx_f32[5] = trikst * cosf(xt2[2] + trikkam );
	
	Fx_f32[6] = 0.0;
  Fx_f32[7] = 0.0;
  Fx_f32[8] = 1.0;
	
	Fu_f32[0] = 0.5 * cosf(xt2[2] + trikkam );
  Fu_f32[1] = 0.5 * sinf(xt2[2] + trikkam );
  Fu_f32[2] = 0.5 * cosf(xt2[2] + trikkam );
  Fu_f32[3] = 0.5 * sinf(xt2[2] + trikkam );
	Fu_f32[4] = 1/b;
	Fu_f32[5] = 1/b;
	
	//FxT=Fx'
	arm_mat_trans_f32(&Fx, &FxT);
	//FuT=Fu'
	arm_mat_trans_f32(&Fu, &FuT);
	//A=Fx*PP
	arm_mat_mult_f32(&Fx, &PP, &A);
	//AA=Fx'*A
	arm_mat_mult_f32(&A, &FxT, &AA);
	//B=Fu*Q
	arm_mat_mult_f32(&Fu, &Q, &B);
	//BB=Fu'*B
	arm_mat_mult_f32(&B, &FuT, &BB);
	//P=Fx*Pt2*Fx'+Fu*Q*Fu';
	//P=AA+BB;
	arm_mat_add_f32(&AA, &BB, &P);
	
// matavimas

skirx=-zyme_f32[0]+xt[0];
skiry=-zyme_f32[1]+xt[1];
skirxk=skirx*skirx;
skiryk=skiry*skiry;
nnn=skiry/skirx;

zz[0]=sqrt(skirxk+skiryk);
zz[1]=atan(nnn)-xt[2];
//zz[1]=0;

H_f32[0]=2*skirx/(2*sqrt(skirxk + skiryk));
H_f32[1]=2*skiry/(2*sqrt(skirxk + skiryk));
H_f32[2]=0;
H_f32[3]=skiry/(skirxk*((skiryk/skirxk)+1));
H_f32[4]=1/(skirxk*((skiryk/skirxk)+1));
H_f32[5]=-1;


V_f32[0]=1;
V_f32[1]=0;
V_f32[2]=0;
V_f32[3]=1;

	//HT=H'
	arm_mat_trans_f32(&H, &HT);
	//VT=V'
	arm_mat_trans_f32(&V, &VTr);
	//N=H*P
	arm_mat_mult_f32(&H, &P, &N);
	//NN=N*H'
	arm_mat_mult_f32(&N, &HT, &NN);
	//M=V*R
	arm_mat_mult_f32(&V, &R, &M);
	//MM=M*V'
	arm_mat_mult_f32(&M, &VTr, &MM);
	//S=NN+MM;
	arm_mat_add_f32(&MM, &NN, &S);
	arm_mat_add_f32(&MM, &NN, &SS);

vt_f32[0]=lidar_l-zz[0];
vt_f32[1]=lidar_kampas-zz[1];

//L=P*H'
arm_mat_mult_f32(&P, &HT, &L);
//inv(S)
arm_mat_inverse_f32(&S, &SINV);
//K=L*Sinv
arm_mat_mult_f32(&L, &SINV, &K);


//J=K*vt;
arm_mat_mult_f32(&K, &vt, &J);

xxx[0]=xt[0]+J_f32[0];
XKalman[kk]=xxx[0];
xxx[1]=xt[1]+J_f32[1];
YKalman[kk]=xxx[1];
xxx[2]=xt[2]+J_f32[2];
kamKalman[kk]=xxx[2];

xt2[0]=xt[0];
xt2[1]=xt[1];
xt2[2]=xt[2];

//K'
arm_mat_trans_f32(&K, &KT);
//W=K*S
arm_mat_mult_f32(&K, &SS, &W);
//WW=Q*K'
arm_mat_mult_f32(&W, &KT, &WW);
//PP=P-QQ
arm_mat_sub_f32(&P, &WW, &PP);


kk++;
}
//








