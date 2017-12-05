#include "dstar.h"
#include "string.h"
#include "debuguart.h"
#include "zingsniniai_varikliai.h"

dstar_t dstar={
  .goal_x=0,
	.goal_y=0,
};


void dstar_reset_map(void)
{
  memset(&dstar.map_g,DSTAR_FREE,sizeof(dstar.map_g));
	memset(&dstar.map_rhs,DSTAR_FREE,sizeof(dstar.map_g)); //reset veikia
	memset(&dstar.open_st,DSTAR_ONPEN_NO,sizeof(dstar.open_st));
	memset(&dstar.open_path_st,DSTAR_ONPEN_NO,sizeof(dstar.open_path_st));
	dstar.command_count=0;
	//for (uint16_t i=0;i<sizeof(dstar.open_path_st);i++)
	//{
	//   PRINTF("%u",dstar.open_path_st[i]);
	//}
  //PRINTF("dsatr %u\r\n",dstar.map_g[0][0]);
	//PRINTF("dsatr %u\r\n",dstar.map_g[0][1]);
}

void dstar_add_obstacle(uint16_t x,uint16_t y)
{
  if(x<DSTAR_MAP_SIZE && y<DSTAR_MAP_SIZE)
	{
	   //jeigu neiseina is ribu dedam
		dstar.map_g[x][y]=DSTAR_OBSTACLE;
		dstar.map_rhs[x][y]=DSTAR_OBSTACLE;
		//PRINTF("dstar obs: %u %u %x %x\r\n",x,y,dstar.map_rhs[x][y],dstar.map_g[x][y]);
	}
	else
	{
	  //obstacle
		//PRINTF("dstar out of bound\r\n");
	
	}

}


uint8_t dstar_plan(uint8_t goalx, uint8_t goaly, uint8_t start_x, uint8_t start_y)
{
	  dstar.start_x=start_x;
	  dstar.start_y=start_y;
	  dstar.goal_x=goalx;
	  dstar.goal_y=goaly;
	  dstar.count_no=0;
	  uint8_t return_code;
    //planuojam kelia
	  //atidarom goal jei nekliutis jei kliutis grazinam golo kliuties koda
	  if (dstar.map_g[goalx][goaly]==DSTAR_OBSTACLE)
		{
		  return DSTAR_PLAN_GOAL_OBSTACLE;
		}
		
		//kitu atveju atidarom gola
		dstar_open(goalx, goaly);
		//setinam rhs i nulis
		dstar.map_rhs[goalx][goaly]=0;
		
		while(1)
		{
			//
			dstar.count_no++;
			return_code=find_min_open_and_procead();
			if (return_code==DSTAR_PROC_START)
			{
			  return DSTAR_PLAN_OK;
			}
			else if (return_code==DSTAR_PROC_NOLIST)
			{
			  return DSTAR_PLAN_NOLIST;
			}
			if(dstar.count_no>=DSTAT_MAX_CYCLE)
			{
			  return DSTAR_PLAN_CYCLE_ER;
			}
		  
		}
		
    return DSTAR_PLAN_CYCLE_ER;
}

uint8_t dstar_plan_command(float start_kampas)
{
	  dstar.count_no=0;
	  uint8_t return_code;
	  dstar.command_count=0;
    //planuojam kelia
	  //atidarom start jei nekliutis jei kliutis grazinam golo kliuties koda
		
		//kitu atveju atidarom gola
	  //uint8_t dstar_make_path(uint8_t start_x, uint8_t start_y)
	  //atidarom start
	  PRINTF("open %u %u\r\n",dstar.start_x,dstar.start_y);
	  dstar_open_path(dstar.start_x, dstar.start_y, 0, start_kampas);
		//dstar_open_(goalx, goaly);
		//setinam rhs i nulis
		//dstar.map_rhs[goalx][goaly]=0;
		
		while(1)
		{
			//
			dstar.count_no++;
			return_code=dstar_make_path();
			if (return_code==DSTAR_PATH_OK)
			{
			  return DSTAR_PATH_OK;
			}
			
			if(dstar.count_no>=DSTAT_MAX_CYCLE)
			{
			  return DSTAR_PATH_ERROR;
			}
		  
		}
		
    return DSTAR_PATH_ERROR;
}

void dstar_open(uint8_t x, uint8_t y)
{
	for (uint8_t i=0;i<MAX_OPEN_LIST;i++)
	{
		//atidarom laisva lista
		if (dstar.open_st[i]==DSTAR_ONPEN_NO)
		{
		  dstar.open_x[i]=x;
			dstar.open_y[i]=y;
			//pazymim atidarima
			dstar.open_st[i]=DSTAR_ONPEN_YES;
			//PRINTF("open %u %u %u",x,y,i);
			break;
			
		}
	
	}
  
}

uint8_t find_min_open_and_procead(void)
{
    //surandame maziausia atidaryta
	  uint8_t min_indx=255;
	  uint16_t min=0xFFFF;
	
	  
	  for (uint8_t i=0;i<MAX_OPEN_LIST;i++)
	  {
			if (dstar.open_st[i]==DSTAR_ONPEN_YES)
			{
				//jeigu rhs maziausias
			  if (dstar.map_rhs[dstar.open_x[i]][dstar.open_y[i]]<min)
				{
					min=dstar.map_rhs[dstar.open_x[i]][dstar.open_y[i]];
					min_indx=i;
				}
				
			}
			
		}
		//pasirenkam maziausia rhs ir jei g>rhs keiciam
		if (min_indx!=255)
		{
				if (dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]>dstar.map_rhs[dstar.open_x[min_indx]][dstar.open_y[min_indx]])
				{
					dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]=dstar.map_rhs[dstar.open_x[min_indx]][dstar.open_y[min_indx]];
					//isimam is listo 
					
					
					if (dstar.open_x[min_indx]==dstar.start_x && dstar.open_y[min_indx]==dstar.start_y)
					{
					  //grazinam kad radom kelia
						return DSTAR_PROC_START;
					}
					//PRINTF("fffff %u\r\n",dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]);
					//exposinam aplink nexpozintus ir ne obstacle laukelius ji free laukelius
					// \ | /
					// -   -
					// / | \
					//devinios kryptys
					//1<-
					if (dstar.open_x[min_indx]>0) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]-1][dstar.open_y[min_indx]]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]-1][dstar.open_y[min_indx]]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAR_FORWARD;
							dstar_open(dstar.open_x[min_indx]-1, dstar.open_y[min_indx]);
						}
						
					}
					
					//2 ^\
					
					if (dstar.open_x[min_indx]>0 && dstar.open_y[min_indx]<(DSTAR_MAP_SIZE-1)) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]-1][dstar.open_y[min_indx]+1]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]-1][dstar.open_y[min_indx]+1]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAT_DIAGNOL;
							dstar_open(dstar.open_x[min_indx]-1, dstar.open_y[min_indx]+1);
						}
						
					}
					//----------------------------------------------------------------------------------
					
					//3 ^|
					
					if (dstar.open_y[min_indx]<(DSTAR_MAP_SIZE-1)) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]][dstar.open_y[min_indx]+1]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]][dstar.open_y[min_indx]+1]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAR_FORWARD;
							dstar_open(dstar.open_x[min_indx], dstar.open_y[min_indx]+1);
						}
						
					}
					//----------------------------------------------------------------------------------
					//4 /^
					
					if (dstar.open_x[min_indx]<(DSTAR_MAP_SIZE-1) && dstar.open_y[min_indx]<(DSTAR_MAP_SIZE-1)) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]+1][dstar.open_y[min_indx]+1]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]+1][dstar.open_y[min_indx]+1]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAT_DIAGNOL;
							dstar_open(dstar.open_x[min_indx]+1, dstar.open_y[min_indx]+1);
						}
						
					}
					//----------------------------------------------------------------------------------
					
					//5 ->
					
					if (dstar.open_x[min_indx]<(DSTAR_MAP_SIZE-1) ) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]+1][dstar.open_y[min_indx]]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]+1][dstar.open_y[min_indx]]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAR_FORWARD;
							dstar_open(dstar.open_x[min_indx]+1, dstar.open_y[min_indx]);
						}
						
					}
					//----------------------------------------------------------------------------------
					//6 \.
					
					if (dstar.open_x[min_indx]<(DSTAR_MAP_SIZE-1) && dstar.open_y[min_indx]>0) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]+1][dstar.open_y[min_indx]-1]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]+1][dstar.open_y[min_indx]-1]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAT_DIAGNOL;
							dstar_open(dstar.open_x[min_indx]+1, dstar.open_y[min_indx]-1);
						}
						
					}
					//----------------------------------------------------------------------------------
					//7 |.
					
					if ( dstar.open_y[min_indx]>0) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]][dstar.open_y[min_indx]-1]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]][dstar.open_y[min_indx]-1]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAR_FORWARD;
							dstar_open(dstar.open_x[min_indx], dstar.open_y[min_indx]-1);
						}
						
					}
					//----------------------------------------------------------------------------------
					//8 /.
					
					if (dstar.open_x[min_indx]>0 && dstar.open_y[min_indx]>0) 
					{ 
						if(dstar.map_rhs[dstar.open_x[min_indx]-1][dstar.open_y[min_indx]-1]==DSTAR_FREE)
						{
							//
							dstar.map_rhs[dstar.open_x[min_indx]-1][dstar.open_y[min_indx]-1]=dstar.map_g[dstar.open_x[min_indx]][dstar.open_y[min_indx]]+DSTAT_DIAGNOL;
							dstar_open(dstar.open_x[min_indx]-1, dstar.open_y[min_indx]-1);
						}
						
					}
					//----------------------------------------------------------------------------------
					dstar.open_st[min_indx]=DSTAR_ONPEN_NO;
				}
		   return DSTAR_PROC_LIST;
		}
		else
		{
		  return DSTAR_PROC_NOLIST; 
		}
			
	

}

void dstar_open_path(uint8_t x, uint8_t y,float atstumas, float kampas)
{
	if (x<DSTAR_MAP_SIZE && y<DSTAR_MAP_SIZE)
	{
		for (uint8_t i=0;i<OPEN_PATH_SIZE;i++)
	{
		//atidarom laisva lista
		if (dstar.open_path_st[i]==DSTAR_ONPEN_NO)
		{
		  dstar.open_path_x[i]=x;
			dstar.open_path_y[i]=y;
			dstar.opened_atstumas[i]=atstumas;
			if (kampas>=360)
			{
			  kampas=kampas-360;
			}
			dstar.opened_kampas[i]=kampas;
			//pazymim atidarima
			dstar.open_path_st[i]=DSTAR_ONPEN_YES;
			//PRINTF("open %u %u %u",x,y,i);
			break;
			
		}
	
	}
	
	}
	
  
}


uint8_t dstar_make_path(void)
{
	  uint8_t min_indx=255;
	  uint16_t min=0xFFF0;
	
	  
	  for (uint8_t i=0;i<OPEN_PATH_SIZE;i++)
	  {
			if (dstar.open_path_st[i]==DSTAR_ONPEN_YES)
			{
				//jeigu rhs maziausias
			  if (dstar.map_g[dstar.open_path_x[i]][dstar.open_path_y[i]]<min)
				{
					min=dstar.map_g[dstar.open_path_x[i]][dstar.open_path_y[i]];
					min_indx=i;
					
				}
				
			}
			
		}
		//PRINTF("min %u x %u y %u\r\n",min,dstar.open_path_x[min_indx],dstar.open_path_y[min_indx] );
		//uzdarom visus tik ne min
		memset(&dstar.open_path_st,DSTAR_ONPEN_NO,sizeof(dstar.open_path_st));
		dstar.open_path_st[min_indx]=DSTAR_ONPEN_YES;
		
		//dedam komandas i comandu buferi
		dstar.command_kampas[dstar.command_count]=dstar.opened_kampas[min_indx];
		dstar.command_atstumas[dstar.command_count++]=dstar.opened_atstumas[min_indx];
		
		if (min_indx!=255)
		{
			//atidarom visus aplinkui
			//jeigu tai galutinis taskas grazinam kelia
			     if (dstar.open_path_x[min_indx]==dstar.goal_x && dstar.open_path_y[min_indx]==dstar.goal_y)
			    {
					  //grazinam kad radom kelia
						return DSTAR_PATH_OK;
			    }
			//kitu atveju atidarom aplinkui esancius laukelius
					// \ | /
					// -   -
					// / | \
					//devinios kryptys
					//1<-
					if (dstar.open_path_x[min_indx]>0) 
					{ 
							dstar_open_path(dstar.open_path_x[min_indx]-1, dstar.open_path_y[min_indx],DSTAR_FORWARD,dstar.kampas_start+270);
						  //dstar.opened_atstumas[min_indx]
					}
					//2 ^\

					if (dstar.open_path_x[min_indx]>0 && dstar.open_path_y[min_indx]<(DSTAR_MAP_SIZE-1)) 
					{ 
							dstar_open_path(dstar.open_path_x[min_indx]-1, dstar.open_path_y[min_indx]+1,DSTAT_DIAGNOL,dstar.kampas_start+225);
					}
					//----------------------------------------------------------------------------------
					
					//3 ^|
					if (dstar.open_path_y[min_indx]<(DSTAR_MAP_SIZE-1)) 
					{ 
							dstar_open_path(dstar.open_path_x[min_indx], dstar.open_path_y[min_indx]+1,DSTAR_FORWARD,dstar.kampas_start+180);
					}
					//----------------------------------------------------------------------------------
					//4 /^
					
					if (dstar.open_path_x[min_indx]<(DSTAR_MAP_SIZE-1) && dstar.open_path_y[min_indx]<(DSTAR_MAP_SIZE-1)) 
					{
							dstar_open_path(dstar.open_path_x[min_indx]+1, dstar.open_path_y[min_indx]+1,DSTAT_DIAGNOL,dstar.kampas_start+135);
					}
					//----------------------------------------------------------------------------------
					
					//5 ->
					if (dstar.open_path_x[min_indx]<(DSTAR_MAP_SIZE-1) ) 
					{
							dstar_open_path(dstar.open_path_x[min_indx]+1, dstar.open_path_y[min_indx],DSTAR_FORWARD,dstar.kampas_start+90);
					}
					//----------------------------------------------------------------------------------
					//6 \.
					
					if (dstar.open_path_x[min_indx]<(DSTAR_MAP_SIZE-1) && dstar.open_path_y[min_indx]>0) 
					{
							dstar_open_path(dstar.open_path_x[min_indx]+1, dstar.open_path_y[min_indx]-1,DSTAT_DIAGNOL,dstar.kampas_start+45);
					}
					//----------------------------------------------------------------------------------
					//7 |.
					
					if ( dstar.open_path_y[min_indx]>0) 
					{
							dstar_open_path(dstar.open_path_x[min_indx], dstar.open_path_y[min_indx]-1,DSTAR_FORWARD,dstar.kampas_start+0);
					}
					//----------------------------------------------------------------------------------
					//8 /.
					
					if (dstar.open_path_x[min_indx]>0 && dstar.open_path_y[min_indx]>0) 
					{
							dstar_open_path(dstar.open_path_x[min_indx]-1, dstar.open_path_y[min_indx]-1,DSTAT_DIAGNOL,dstar.kampas_start+315);
					}
					//----------------------------------------------------------------------------------
					dstar.open_path_st[min_indx]=DSTAR_ONPEN_NO;
		}
	  return DSTAR_PATH_PLAN;


}



void dstar_printf_map(void)
{
	PRINTF("DSTAR plan\r\n");
	for (uint8_t i=0;i<DSTAR_MAP_SIZE;i++)
	{
		for (uint8_t j=0;j<DSTAR_MAP_SIZE;j++)
			{
	
				PRINTF("%04x ",dstar.map_g[j][i]);
			}
	
	  PRINTF("\r\n");
	}
   
}

void dstar_printf_path(void)
{
	PRINTF("DSTAR path count %u\r\n",dstar.command_count);
	for (uint8_t i=0;i<dstar.command_count;i++)
	{
		PRINTF("%u: ats: %f kampas: %f\r\n",i,dstar.command_atstumas[i],dstar.command_kampas[i]);
	 // PRINTF("\r\n");
	}
	PRINTF("---------------------------\r\n");
   
}


void dstar_set_kampas_start(float kampas_startass)
{
  dstar.kampas_start=kampas_startass;
}

void dstar_push_command_to_ZV(void)
{
	for (uint8_t i=0;i<dstar.command_count;i++)
	{
		//PRINTF("%u: ats: %f kampas: %f\r\n",i,dstar.command_atstumas[i],dstar.command_kampas[i]);
	 // PRINTF("\r\n");
		 zv_task_push(dstar.kampas_start+dstar.command_kampas[i], dstar.command_atstumas[i],dstar.command_kampas[i]);
	}
  // zv_task_push(230, 500);

}